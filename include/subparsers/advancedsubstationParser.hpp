#include <common.hpp>

namespace vmp { namespace subs {

    enum ASSSections { // Taken from Spec
        ASS_NONE = 0,
        ASS_SCRIPT_INFO,
        ASS_STYLES,
        ASS_STYLES_PLUS,
        ASS_EVENTS,
        ASS_FONTS,
        ASS_GRAPHICS
    };

    struct ASSStyleV4P { // Taken from Spec
        std::string name; // Style Name
        std::string fontName; // Font to use
        int fontSize; // Font Size
        SceUInt primaryColour; // ABGR
        SceUInt secondaryColour; // ABGR
        SceUInt outlineColour; // ABGR
        SceUInt backColour; // ABGR
        int bold; // -1 true, 0 false
        int italic; // -1 true, 0 false
        int underline; // -1 true, 0 false
        int strikeout; // -1 true, 0 false
        int scaleX; // Font Width in Percent
        int scaleY; // Font Height in Percent
        int spacing; // Space between pixels in pixels
        float angle; // Can be treated as float. I simply made it a float. Measured in Degrees
        int borderStyle; // 1 = Outline + drop shadow, 3 = Opaque box
        int outline; // If BorderStyle is 1,  then this specifies the width of the outline around the text, in pixels. Values may be 0, 1, 2, 3 or 4
        int shadow; // If BorderStyle is 1,  then this specifies the depth of the drop shadow behind the text, in pixels. Values may be 0, 1, 2, 3 or 4. Drop shadow is always used in addition to an outline
        int alignment; // Alignment after the layout of a numpad (1-3 sub, 4-6 mid, 7-9 top).
        int marginL; // This defines the Left Margin in pixels. It is the distance from the left-hand edge of the screen.The three onscreen margins (MarginL, MarginR, MarginV) define areas in which the subtitle text will be displayed
        int marginR; // This defines the Right Margin in pixels. It is the distance from the right-hand edge of the screen. The three onscreen margins (MarginL, MarginR, MarginV) define areas in which the subtitle text will be displayed
        int marginV; // This defines the vertical Left Margin in pixels. For a subtitle, it is the distance from the bottom of the screen. For a toptitle, it is the distance from the top of the screen. For a midtitle, the value is ignored - the text will be vertically centred
        int encoding; // Expect this to be 0
    };

    struct ASSDialogue { // Taken from Spec
        int layer; // Higher numbered layers will be drawn over the lower numbered.  
        std::string start; // Hour:Min:Sec.MS (Spec says Hour:Min:Sec:MS)
        std::string end; // Hour:Min:Sec.MS (Spec says Hour:Min:Sec:MS)
        std::string style; // ASSStyle Name
        std::string name; // Character name of the person speaking
        int marginL; // 4-figure Left Margin override. The values are in pixels. All zeroes means the default margins defined by the style are used.
        int marginR; // 4-figure Right Margin override. The values are in pixels. All zeroes means the default margins defined by the style are used.
        int marginV; // 4-figure Bottom Margin override. The values are in pixels. All zeroes means the default margins defined by the style are used.
        std::string effect; // May not use this one right now. Has some good stuff to choose though
        std::string text; // Subtitle text (Can have commas)
    };

    class ASSParser : public SubtitleParser
    {
            void parse(std::string fileName);
        private:
            std::string _title;
            std::string _originalScript;
            std::string _updateDetails;
            std::string _scriptType;
            std::string _collisions;
            int _playResX;
            int _playResY;
            int _playDepth;
            double _timer;
            int _wrapStyle;

            std::vector<ASSStyleV4P> _styles;
            std::vector<ASSDialogue> _dialogue;

            void setTitle(std::string title);
            void setOriginalScript(std::string originalScript);
            void setUpdateDetails(std::string updateDetails);
            void setScriptType(std::string scriptType);
            void setCollisions(std::string collisions);
            void populateStyle(std::string style);
            void populateDialogue(int subNo, std::string dialogue);

        public:
            ASSParser(void);
            ASSParser(std::string fileName);
            ~ASSParser(void);
    };

    inline void ASSParser::setTitle(std::string title)
    {
        _title = title;
        _title.erase(_title.find_last_not_of(" \t\n\r\f\v") + 1);
    }

    inline void ASSParser::setOriginalScript(std::string originalScript)
    {
        _originalScript = originalScript;
        _originalScript.erase(_originalScript.find_last_not_of(" \t\n\r\f\v") + 1);
    }

    inline void ASSParser::setUpdateDetails(std::string updateDetails)
    {
        _updateDetails = updateDetails;
        _updateDetails.erase(_updateDetails.find_last_not_of(" \t\n\r\f\v") + 1);
    }

    inline void ASSParser::setScriptType(std::string scriptType)
    {
        _scriptType = scriptType;
        _scriptType.erase(_scriptType.find_last_not_of(" \t\n\r\f\v") + 1);
        std::transform(_scriptType.begin(), _scriptType.end(), _scriptType.begin(), std::toupper);
    }

    inline void ASSParser::setCollisions(std::string collisions)
    {
        _collisions = collisions;
        _collisions.erase(_collisions.find_last_not_of(" \t\n\r\f\v") + 1);
    }

    inline void ASSParser::populateStyle(std::string style)
    {
        ASSStyleV4P outStyle;

        std::vector<std::string> styleSplit;
        styleSplit = split(style, ',', styleSplit, 23);

        outStyle.name = styleSplit[0];
        outStyle.fontName = styleSplit[1];
        outStyle.fontSize = stoi(styleSplit[2]);
        outStyle.primaryColour = stoi(styleSplit[3].substr(2, 8), 0, 16);
        outStyle.secondaryColour = stoi(styleSplit[4].substr(2, 8), 0, 16);
        outStyle.outlineColour = stoi(styleSplit[5].substr(2, 8), 0, 16);
        outStyle.backColour = stoi(styleSplit[6].substr(2, 8), 0, 16);
        outStyle.bold = stoi(styleSplit[7]);
        outStyle.italic = stoi(styleSplit[8]);
        outStyle.underline = stoi(styleSplit[9]);
        outStyle.strikeout = stoi(styleSplit[10]);
        outStyle.scaleX = stoi(styleSplit[11]);
        outStyle.scaleY = stoi(styleSplit[12]);
        outStyle.spacing = stoi(styleSplit[13]);
        outStyle.angle = stof(styleSplit[14]);
        outStyle.borderStyle = stoi(styleSplit[15]);
        outStyle.outline = stoi(styleSplit[16]);
        outStyle.shadow = stoi(styleSplit[17]);
        outStyle.alignment = stoi(styleSplit[18]);
        outStyle.marginL = stoi(styleSplit[19]);
        outStyle.marginR = stoi(styleSplit[20]);
        outStyle.marginV = stoi(styleSplit[21]);
        outStyle.encoding = stoi(styleSplit[22]);

        _styles.push_back(outStyle);

    }

    inline void ASSParser::populateDialogue(int subNo, std::string dialogue)
    {
        ASSDialogue outDialogue;

        std::vector<std::string> dialogueSplit;

        // while(dialogue.find("\N") != std::string::npos) { // Some pre-cleaning before sending to SubtitleItem
        //     dialogue.replace(dialogue.find("\N") - 1, 2, "\n");
        // }

        dialogueSplit = split(dialogue, ',', dialogueSplit, 10);

        outDialogue.layer = stoi(dialogueSplit[0]);
        outDialogue.start = dialogueSplit[1];
        outDialogue.end = dialogueSplit[2];
        outDialogue.style = dialogueSplit[3];
        outDialogue.name = dialogueSplit[4];
        outDialogue.marginL = stoi(dialogueSplit[5]);
        outDialogue.marginR = stoi(dialogueSplit[6]);
        outDialogue.marginV = stoi(dialogueSplit[7]);
        outDialogue.effect = dialogueSplit[8];
        outDialogue.text = dialogueSplit[9].size() < 1024 ? dialogueSplit[9] : "";

        while(outDialogue.text.find("\\N") != std::string::npos) { // Some pre-cleaning before sending to SubtitleItem
            outDialogue.text.replace(outDialogue.text.find("\\N"), 2, "\n");
        }
        
        _dialogue.push_back(outDialogue);

        _subtitles.push_back(new SubtitleItem(subNo, outDialogue.start, outDialogue.end, outDialogue.text));

    }

    inline ASSParser::ASSParser(void)
    {
    }

    inline void ASSParser::parse(std::string fileName)      //ass parser
    {

        std::ifstream infile(fileName);
        std::string line, start, end, completeLine = "", timeLine = "";
        int subNo, styleNo = 0;
        ASSSections currentSection = ASS_NONE;

        while (std::getline(infile, line))
        {
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());

            if (line.compare(""))
            {
                if (line.at(0) == ';') // Comment. Ignore
                    continue;

                if (line.at(0) == '[')
                {
                    std::string section = line.substr(1, line.find_first_of(']') - 1);

                    std::transform(section.begin(), section.end(), section.begin(), std::toupper); // Yeah. The spec doesn't seem to say anything about case, so this is for saftey

                    if (!section.compare("SCRIPT INFO")) // YandereDev mode activated
                        currentSection = ASS_SCRIPT_INFO;
                    else if (!section.compare("V4 STYLES"))
                        currentSection = ASS_STYLES;
                    else if (!section.compare("V4+ STYLES"))
                        currentSection = ASS_STYLES_PLUS;
                    else if (!section.compare("EVENTS"))
                        currentSection = ASS_EVENTS;
                    else if (!section.compare("FONTS"))
                        currentSection = ASS_FONTS;
                    else if (!section.compare("GRAPHICS"))
                        currentSection = ASS_GRAPHICS;
                    else
                        currentSection = ASS_NONE;

                    continue;
                }

                if (currentSection != ASS_NONE)
                {
                    /** Handle Script Info **/
                    if (currentSection == ASS_SCRIPT_INFO)
                    {
                        std::vector<std::string> header;
                        header = split(line, ':', header); // Each line will always have a header terminated with ':'
                        if (!header[0].compare("Title")) // YandereDev mode activated again
                            setTitle(header[1]);
                        else if (!header[0].compare("Original Script"))
                            setOriginalScript(header[1]);
                        else if (!header[0].compare("Update Details"))
                            setUpdateDetails(header[1]);
                        else if (!header[0].compare("ScriptType"))
                            setUpdateDetails(header[1]);
                        else if (!header[0].compare("Collisions"))
                            setCollisions(header[1]);
                        else if (!header[0].compare("PlayResX"))
                            _playResX = stoi(header[1]);
                        else if (!header[0].compare("PlayResY"))
                            _playResY = stoi(header[1]);
                        else if (!header[0].compare("PlayDepth"))
                            _playDepth = stoi(header[1]);
                        else if (!header[0].compare("Timer"))
                            _timer = stod(header[1]);
                        else if (!header[0].compare("WrapStyle"))
                            _wrapStyle = stoi(header[1]);

                    }

                    /** Handle V4+ Styles **/
                    if (currentSection == ASS_STYLES_PLUS)
                    {
                        std::vector<std::string> header;
                        header = split(line, ':', header);
                        if (!header[0].compare("Style"))
                            populateStyle(header[1]);
                    }

                    /** Handle Events **/
                    if (currentSection == ASS_EVENTS)
                    {
                        std::string header;
                        header = line.substr(0, line.find(": "));
                        if (!header.compare("Dialogue"))
                        {
                            header = line.substr(10);
                            subNo++;
                            populateDialogue(subNo, header);
                        }
                    }
                }
            }

        }
    }

    inline ASSParser::ASSParser(std::string fileName)
    {
        _fileName = fileName;
        parse(fileName);
    }

    inline ASSParser::~ASSParser(void)
    {
        for(int i=0;i != _subtitles.size();++i)
        {
            if(_subtitles[i])
                delete _subtitles[i];
        }
    }
}};
