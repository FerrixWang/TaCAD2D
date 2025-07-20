#ifndef _SketchEditorParameterSet_Header_File_
#define _SketchEditorParameterSet_Header_File_

#include "Comm/ParameterSets.hxx"

class SketchEditorParameterSet : public OverridableParameterSet
{
    Q_OBJECT
public:
    explicit SketchEditorParameterSet(const QString& group, QObject* parent = nullptr)
        : OverridableParameterSet(group, parent)
    {
        SetDefaultValue("DeviationAngle", 2.0);
        SetDefaultValue("SegmentSelectionSensitivity", 1.0);
        SetDefaultValue("MaximumPointCountSnapping", 10);
    }

    double DeviationAngle() const { return GetValue<double>("DeviationAngle"); }
    void SetDeviationAngle(double value) { SetValue("DeviationAngle", value); }

    double SegmentSelectionSensitivity() const { return GetValue<double>("SegmentSelectionSensitivity"); }
    void SetSegmentSelectionSensitivity(double value) { SetValue("SegmentSelectionSensitivity", value); }

    int MaximumPointCountSnapping() const { return GetValue<int>("MaximumPointCountSnapping"); }
    void SetMaximumPointCountSnapping(int value) { SetValue("MaximumPointCountSnapping", value); }
};

#endif // _SketchEditorParameterSet_Header_File_
