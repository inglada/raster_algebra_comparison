#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "otbMultiToMonoChannelExtractROI.h"
#include "itkNaryAddImageFilter.h"
#include "itkMultiplyImageFilter.h"

namespace otb
{

namespace Wrapper
{
class TimeAverage : public Application
{
public:
  typedef TimeAverage Self;
  typedef Application Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  itkNewMacro(Self);
  itkTypeMacro(TimeAverage, otb::Application);

  // Image and pixel types
  typedef otb::Image<float, 2>               ImageType;
  typedef ImageType::InternalPixelType       ImagePixelType;
  typedef FloatVectorImageType               VectorImageType;
  typedef VectorImageType::InternalPixelType VectorImagePixelType;

  // Filters and iterators
  typedef otb::MultiToMonoChannelExtractROI<VectorImagePixelType, ImagePixelType>  ExtractChannelType;
  typedef itk::NaryAddImageFilter<ImageType, ImageType>                            NaryAddImageFilterType;
  typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType>                MultiplyImageFilterType;

private:
  void DoInit()
  {
    SetName("TimeAverage");
    SetDescription("");
    SetDocName("");
    SetDocLongDescription("");
    SetDocLimitations("None");
    SetDocAuthors("");
    SetDocSeeAlso("");
    AddDocTag(Tags::Analysis);
    AddDocTag("");

    AddParameter(ParameterType_InputImageList, "il", "Input images");
    AddParameter(ParameterType_OutputFilename, "out", "Output Image");

    // System parameters
    AddParameter(ParameterType_RAM, "ram", "Available RAM");
    SetDefaultParameterInt("ram", 256);
    MandatoryOff("ram");
  }

  void DoUpdateParameters()
  {
  }

  void DoExecute()
  {
    FloatVectorImageListType::Pointer inList = GetParameterImageList("il");

    std::vector<ExtractChannelType::Pointer> extractChannelFilters(inList->Size(), NULL);
    NaryAddImageFilterType::Pointer naryAdder = NaryAddImageFilterType::New();

    // For each input image
    for (unsigned int i = 0; i < inList->Size(); i++)
    {
      extractChannelFilters[i] = ExtractChannelType::New();
      extractChannelFilters[i]->SetInput(inList->GetNthElement(i));

      // Extract first band
      extractChannelFilters[i]->SetChannel(1);

      // Add it
      naryAdder->SetInput(0, extractChannelFilters[i]->GetOutput());
    }

    // Divide by N
    MultiplyImageFilterType::Pointer divideFilter = MultiplyImageFilterType::New();
    divideFilter->SetInput(naryAdder->GetOutput());
    divideFilter->SetConstant(1.0f/inList->Size());

    // Write
    SetParameterOutputImage("out", divideFilter->GetOutput());
  }
};
} // namespace Wrapper
} //namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::TimeAverage)

