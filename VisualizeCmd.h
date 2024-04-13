#include <maya/MPxCommand.h>

class VisualizeCmd : public MPxCommand {
public:
    virtual MStatus doIt (const MArgList& );
    static void *creator() { return new VisualizeCmd; }
};