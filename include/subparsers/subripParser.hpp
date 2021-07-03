#include <common.hpp>

namespace vmp { namespace subs {

    class SRTParser : public SubtitleParser
    {
            void parse(std::string fileName);
        public:
            SRTParser(void);
            SRTParser(std::string fileName);
            ~SRTParser(void);
    };

    inline SRTParser::SRTParser(void)
    {
    }

    inline void SRTParser::parse(std::string fileName)      //srt parser
    {

        std::ifstream infile(fileName);
        std::string line, start, end, completeLine = "", timeLine = "";
        int subNo, turn = 0;

        /*
        * turn = 0 -> Add subtitle number
        * turn = 1 -> Add string to timeLine
        * turn > 1 -> Add string to completeLine
        */

        while (std::getline(infile, line))
        {
            printf("Yeet\n");
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());

            if (line.compare(""))
            {
                if(!turn)
                {
                    subNo=atoi(line.c_str());
                    turn++;
                    continue;
                }

                if (line.find("-->") != std::string::npos)
                {
                    timeLine += line;

                    std::vector<std::string> srtTime;
                    srtTime = split(timeLine, ' ', srtTime);
                    start = srtTime[0];
                    end = srtTime[2];

                }
                else
                {
                    if (completeLine != "")
                        completeLine += "";

                    completeLine += line + "\n";
                }

                turn++;
            }

            else
            {
                turn = 0;
                _subtitles.push_back(new SubtitleItem(subNo,start,end,completeLine));
                completeLine = timeLine = "";
            }

            if(infile.eof())    //insert last remaining subtitle
            {
                _subtitles.push_back(new SubtitleItem(subNo,start,end,completeLine));
            }
        }

        if (turn)
        {
            _subtitles.push_back(new SubtitleItem(subNo, start, end, completeLine));
        }
    }

    inline SRTParser::SRTParser(std::string fileName)
    {
        _fileName = fileName;
        parse(fileName);
    }

    inline SRTParser::~SRTParser(void)
    {
        for(int i=0;i != _subtitles.size();++i)
        {
            if(_subtitles[i])
                delete _subtitles[i];
        }
    }

}};