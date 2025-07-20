#ifndef SketchSegmentCreator_Header_File_
#define SketchSegmentCreator_Header_File_

#include "Iact/Shapes/Sketch/SketchTool.hxx"

class SketchSegmentCreator : public SketchTool
{
public:
	//! Constructor
	SketchSegmentCreator() = default;

	//! Destructor
	virtual ~SketchSegmentCreator() = default;

	virtual bool OnStart()override
	{
		OpenCommand();
		return false;
	}

	virtual bool Continue(int continueWithPoint)
	{
		OpenCommand();
		return false;
	}
};

#endif // !SketchSegmentCreator_Header_File_
