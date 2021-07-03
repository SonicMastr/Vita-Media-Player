#include <subtitles.hpp>
#include <utils.hpp>

using namespace vmp::subs;
using namespace vmp::utils;

SceUInt64 SubtitleItem::convMSec(std::string value)
{
    std::vector<std::string> t, secs;
    int hours, mins, seconds, milliseconds;

    t = split(value, ':', t);
    hours = atoi(t[0].c_str());
    mins = atoi(t[1].c_str());

    if (t[2].find(',') != std::string::npos) // SRT uses ','
        secs = split(t[2], ',', secs);
    else if (t[2].find('.') != std::string::npos) // ASS uses '.' (Since it's CSV)
        secs = split(t[2], '.', secs);
    seconds = atoi(secs[0].c_str());
    milliseconds = atoi(secs[1].c_str());

    return hours * 3600000 + mins * 60000 + seconds * 1000 + milliseconds;
}

/**
 * SubtitleFactory
 */

SubtitleFactory::SubtitleFactory(std::string fileName, SceBool videoFile, SceBool internal)
{
    if (internal) {
        _internal = internal;
        return;
    }
    if (!videoFile) {
        if (file::sub_is_valid(fileName))
            _fileName = fileName;
    } else {
        _videoFile = videoFile; // God this fucking sucks
        _fileName = fileName;
    }
}

SubtitleParser* SubtitleFactory::getParser(SubtitleType type)
{
    if(_videoFile)
        file::sub_convert(&_fileName, type);
    
    if (type != 0) {
        if (type == SUBTITLE_SRT) {
            return new SRTParser(_fileName);
        }
        if (type == SUBTITLE_ASS)
            return new ASSParser(_fileName);
    }
    return nullptr;
}

SubtitleFactory::~SubtitleFactory(void)
{
}

/**
 * SubtitleParser
 */

std::string SubtitleParser::getFileData()
{
    std::ifstream infile(_fileName);
    std::string allData = "";
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        allData += line + "\n";
    }
    return allData;

}

SubtitleParser::SubtitleParser(void)
{
}

SubtitleParser::~SubtitleParser(void)
{
}

/**
 * SubtitleItem
 */

SubtitleItem::SubtitleItem(void)
{
}

SubtitleItem::SubtitleItem(int subNo, std::string startTime,std::string endTime, std::string text, SceBool ignore,
                           std::string justDialogue, int speakerCount, int nonDialogueCount,
                           int styleTagCount, int wordCount, std::vector<std::string> speaker, std::vector<std::string> nonDialogue,
                           std::vector<std::string> styleTags, std::vector<std::string> word)
{
    _startTime = convMSec(startTime);
    _endTime = convMSec(endTime);
    _text = text;

    _subNo = subNo;
    _startTimeString = startTime;
    _endTimeString = endTime;
    _ignore = ignore;
    _justDialogue = justDialogue;
    _speakerCount = speakerCount;
    _nonDialogueCount = nonDialogueCount;
    _wordCount = wordCount;
    _speaker = speaker;
    _styleTagCount = styleTagCount;
    _styleTag = styleTags;
    _nonDialogue = nonDialogue;
    _word = word;

    extractInfo();
}

void SubtitleItem::extractInfo(SceBool keepHTML, SceBool doNotIgnoreNonDialogues, SceBool doNotRemoveSpeakerNames)   //process subtitle
{
    std::string output = _text;

    //stripping HTML tags
    if(!keepHTML)
    {
        /*
         * TODO : Before erasing, extract the words.
         * std::vector<std::string> getStyleTags();
         * int getStyleTagCount() const;
         * std::vector<std::string> _styleTag;
         * int _styleTagCount;
         */

        int countP = 0;
        for(char& c : output) // replacing <...> with ~~~~
        {
            if(c=='<' || c=='{')
            {
                countP++;
                c = '~';
            }

            else
            {
                if(countP!=0)
                {
                    if(c != '>' && c != '}')
                        c = '~';

                    else if(c == '>' || c == '}')
                    {
                        c = '~';
                        countP--;
                    }
                }
            }
            
        }
    }

    //stripping non dialogue data e.g. (applause)

    if(!doNotIgnoreNonDialogues)
    {
        /*
         * TODO : Before erasing, extract the words.
         * std::vector<std::string> getNonDialogueWords();
         * int getNonDialogueCount() const;
         * std::vector<std::string> _nonDialogue;
         * int _nonDialogueCount;
         */

        int countP = 0;
        for(char& c : output)   // replacing (...) with ~~~~
        {
            if(c=='(')
            {
                countP++;
                c = '~';
            }

            else
            {
                if(countP!=0)
                {
                    if(c != ')')
                        c = '~';

                    else if(c == ')')
                    {
                        c = '~';
                        countP--;
                    }
                }
            }
        }
    }

    output.erase(std::remove(output.begin(), output.end(), '~'), output.end()); // deleting all ~

    //Extracting speaker names
    if(!doNotRemoveSpeakerNames)
    {
        for(int i=0; output[i]!='\0';i++)
        {
            int colonIndex = 0, nameBeginIndex = 0;
            if(output[i]==':')  //speaker found; travel back
            {
                _speakerCount++;
                colonIndex = i;

                int tempIndex = 0, foundEvilColon = 0, continueFlag = 0, spaceBeforeColon = 0;

                if(output[i-1] == ' ')
                    spaceBeforeColon = 2;

                /*
                Possible Cases :
                Elon Musk: Hey Saurabh, you are pretty smart.       // First and Last Name
                Saurabh: *_* What? Elon Musk: Yes!                  // Two names in single line
                Saurabh : OMG OMG!                                  // Space before colon
                Elon: LOL World: LAMAO
                Saurabh: ._.                                        // normal
                 */

                for(int j=i - spaceBeforeColon; j>=0;j--)
                {
                    if(output[j] == '.' || output[j] == '!' || output[j] == ',' || output[j] == '?' || output[j] == '\n'
                       || output[j] == ' ' || j== 0)
                    {

                        if(output[j] == '.' || output[j] == '!' || output[j] == ',' || output[j] == '?' || j == 0)
                        {
                            if((continueFlag && j == 0))
                            {
                                if(!isupper(output[j]))
                                {
                                    nameBeginIndex = tempIndex;
                                    break;
                                }

                                else
                                    tempIndex = j;

                            }

                            else if(j!=0)
                                tempIndex = j + 1;
                        }

                        else if(output[j] == ' ' && isupper(output[j+1]))
                        {
                            tempIndex = j;
                            continueFlag = 1;

                            continue;
                        }

                        else if(output[j] == ' ' && !isupper(output[j+1] && tempIndex == 0))
                        {
                            _speakerCount--;
                            foundEvilColon = 1;
                            break;
                        }

                        nameBeginIndex = tempIndex;
                        break;
                    }
                }

                if(foundEvilColon)
                    continue;

                i = nameBeginIndex; //compensating the removal and changes in index

                //check if there's a space after colon i.e. A: Hello vs A:Hello
                int removeSpace = 0;
                if(output[colonIndex + 1]==' ')
                    removeSpace = 1;

                _speaker.push_back(output.substr(nameBeginIndex, colonIndex - nameBeginIndex));
                output.erase(nameBeginIndex, colonIndex - nameBeginIndex + removeSpace);
            }

        }

    }

    _justDialogue = output;

    // trimming whitespaces
    const char* whiteSpaces = " \t\n\r\f\v";
    _justDialogue.erase(0, _justDialogue.find_first_not_of(whiteSpaces));
    _justDialogue.erase(_justDialogue.find_last_not_of(whiteSpaces) + 1);

    if(_justDialogue.empty() || _justDialogue == " ")
        _ignore = true;

    else
    {
        _word = split(_justDialogue, ' ', _word); //extracting individual words
        _wordCount = _word.size();
    }
}

std::string SubtitleItem::getDialogue(SceBool keepHTML, SceBool doNotIgnoreNonDialogues,  SceBool doNotRemoveSpeakerNames)
{
    if(_justDialogue.empty())
        extractInfo(keepHTML, doNotIgnoreNonDialogues, doNotRemoveSpeakerNames);

    return _justDialogue;
}

SubtitleItem::~SubtitleItem(void)
{
}