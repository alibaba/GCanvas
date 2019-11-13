#define CONTEXT_ES20
#ifndef GBENCHMARK_CASE_H
#define GBENCHMARK_CASE_H
#include <string>
#include <GCanvas.hpp>
class GBenchMarkCase
{
public:
    GBenchMarkCase():caseName(""),ratio(0){}

    GBenchMarkCase(std::string name):caseName(name),ratio(0)
    {

    }

    virtual ~GBenchMarkCase() {}

    inline std::string getCaseName()
    {
        return this->caseName;
    }

    virtual void draw(std::shared_ptr<gcanvas::GCanvas> canvas,int width,int height)=0;
    
private:
    std::string caseName;
    float ratio;
};
#endif