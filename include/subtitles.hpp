/**
 * This file is based off of srtparser.h (https://github.com/saurabhshri/simple-yet-powerful-srt-subtitle-parser-cpp)
 * 
 */

#ifndef SUBTITLES_HPP
#define SUBTITLES_HPP

#include <common.hpp>

namespace vmp
{
    namespace subs
    {
        enum SubtitleType {
            SUBTITLE_NONE = 0,
            SUBTITLE_TT, //Timed Text (Embedded MP4)
            SUBTITLE_SRT, // SubRip (SRT)
            SUBTITLE_ASS // Advanced SubStation (ASS)
        };

        class SubtitleItem
        {
            private:
                SceUInt64 _startTime;
                SceUInt64 _endTime;
                std::string _text;
                SceUInt64 convMSec(std::string time);

                SceInt _subNo;
                std::string _startTimeString;
                std::string _endTimeString;
                SceBool _ignore;                            //should subtitle be ignore; used when the subtitle is empty after processing
                std::string _justDialogue;                  //contains processed subtitle - stripped style, non dialogue text removal etc.
                SceInt _speakerCount;                       //count of number of speakers
                std::vector<std::string> _speaker;          //list of speakers in a single subtitle
                SceInt _nonDialogueCount;                   //count of non spoken words in a subtitle
                std::vector<std::string> _nonDialogue;      //list of non dialogue words, e.g. (applause)
                SceInt _wordCount;                          //number of words in _justDialogue
                std::vector<std::string> _word;             //list of words in dialogue
                std::vector<SceUInt64> _wordStartTime;      //start time of each word in dialogue
                std::vector<SceUInt64> _wordEndTime;        //end time of each word in dialogue
                std::vector<SceUInt64> _wordDuration;       //actual duration of each word without silence
                SceInt _styleTagCount;                      //count of style tags in a single subtitle
                std::vector<std::string> _styleTag;         //list of style tags in that subtitle
                void extractInfo(SceBool keepHTML = 0, SceBool doNotIgnoreNonDialogues = 0,  SceBool doNotRemoveSpeakerNames = 0);   //process subtitle
            
            public:
                SceUInt64 getStartTime(void) const { return _startTime; };
                SceUInt64 getEndTime(void) const { return _endTime; };
                std::string getText(void) const { return _text; };

                SceInt getSubNo(void) const { return _subNo; };
                std::string getStartTimeString() const { return _startTimeString; };        //returns sarting time as present in .srt file
                std::string getEndTimeString() const { return _endTimeString; };            //returns ending time as present in .srt file
                SceBool getIgnoreStatus() const { return _ignore ? 1 : 0; };                //returns status, whether the subtitle is ignorable or not after processing
                std::string getDialogue(SceBool keepHTML = 0, SceBool doNotIgnoreNonDialogues = 0,  SceBool doNotRemoveSpeakerNames = 0); //returns processed subtitle
                int getSpeakerCount() const{ return _speakerCount; };                       //return speaker count
                int getNonDialogueCount() const { return _nonDialogueCount; };              //return non dialogue words count
                int getStyleTagCount() const { return _styleTagCount; };                    //return style tags count
                int getWordCount() const { return _wordCount; };                            //return words count (Unused for now)
                std::vector<std::string> getSpeakerNames() { return _speaker; };            //return string vector of speaker names
                std::vector<std::string> getNonDialogueWords() { return _nonDialogue; };;   //return string vector of non dialogue words
                std::vector<std::string> getStyleTags() { return _styleTag; };;             //return string vector of style tags

                SubtitleItem(void);
                SubtitleItem(SceInt subNo, std::string startTime,std::string endTime, std::string text, SceBool ignore = false,
                            std::string justDialogue = "" , SceInt speakerCount = 0, SceInt nonDialogueCount = 0,
                            SceInt styleTagCount = 0, SceInt wordCount = 0, std::vector<std::string> speaker = std::vector<std::string>(),
                            std::vector<std::string> nonDialogue = std::vector<std::string>(),
                            std::vector<std::string> styleTags = std::vector<std::string>(),
                            std::vector<std::string> word = std::vector<std::string>());  //default constructor
                ~SubtitleItem(void);

        };

        class SubtitleParser
        {
            protected:
                std::vector<SubtitleItem*> _subtitles;
                std::string _fileName;
                virtual void parse(std::string fileName) = 0;

            public:
                virtual std::vector<SubtitleItem*> getSubtitles() { return _subtitles; };
                std::string getFileData();
                SubtitleParser(void);
                virtual ~SubtitleParser(void);

        };

        class SubtitleFactory
        {
            private:
                std::string _fileName;
                SceBool _internal;
                SceBool _videoFile;
            public:
                SubtitleParser* getParser(SubtitleType type);
                SubtitleFactory(std::string fileName, SceBool videoFile = false, SceBool internal = false);
                ~SubtitleFactory(void);

        };

        inline SubtitleItem *getCurrentSub(std::vector<SubtitleItem *> &subtitles, SceUInt64 currentTime) {
            for (SubtitleItem* item : subtitles)
            {
                if (currentTime > item->getEndTime())  // If the subtitle already passed
                    continue;
                if (currentTime < item->getEndTime()) {
                    if (currentTime < item->getStartTime())
                        return NULL;    // Sub hasn't started yet (Blank Area)
                    return item;    // Return current sub
                }
            }
            return NULL; // Found nothing
        };

    };

};

#include <subparsers/subripParser.hpp>
#include <subparsers/advancedsubstationParser.hpp>

#endif /* SUBTITLES_HPP */