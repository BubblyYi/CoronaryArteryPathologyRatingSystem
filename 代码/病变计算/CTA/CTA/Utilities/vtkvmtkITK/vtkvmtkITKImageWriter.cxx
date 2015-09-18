/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKImageWriter.cxx $
  Date:      $Date: 2006-12-21 07:21:52 -0500 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/

#include "vtkvmtkITKImageWriter.h"

#include <math.h>
#include "vtkImageExport.h"
#include "vtkImageFlip.h"

#include "itkVTKImageImport.h"
#include "itkImageFileWriter.h"

#include "vtkvmtkITKUtility.h"

vtkStandardNewMacro(vtkvmtkITKImageWriter);

#if 0
// helper function
template <class  TPixelType>
void ITKWriteVTKImage(vtkvmtkITKImageWriter *self, vtkImageData *inputImage, char *fileName, 
                      vtkMatrix4x4* rasToIjkMatrix, TPixelType dummy) {

  typedef  itk::Image<TPixelType, 3> ImageType;

  vtkMatrix4x4 *ijkToRasMatrix = vtkMatrix4x4::New();

  vtkMatrix4x4::Invert(rasToIjkMatrix, ijkToRasMatrix);

  ijkToRasMatrix->Transpose();

  typename ImageType::DirectionType direction;
  typename ImageType::PointType origin;
  direction.SetIdentity();

  double mag[3];
  int i;
  for (i=0; i<3; i++) {
    // normalize vectors
    mag[i] = 0;
    for (int j=0; j<3; j++) {
      mag[i] += ijkToRasMatrix->GetElement(i,j)* ijkToRasMatrix->GetElement(i,j);
    }
    if (mag[i] == 0.0) {
      mag[i] = 1;
    }
    mag[i] = sqrt(mag[i]);
    //if (i == 1) { // Y flip
      //mag[i] = -mag[i];
    //}
  }

  for ( i=0; i<3; i++) {
    int j;
    for (j=0; j<3; j++) {
      ijkToRasMatrix->SetElement(i, j, ijkToRasMatrix->GetElement(i,j)/mag[i]);
    }
  }
  
  
  // ITK image direction are in LPS space
  // convert from ijkToRas to ijkToLps
  vtkMatrix4x4* rasToLpsMatrix = vtkMatrix4x4::New();
  rasToLpsMatrix->Identity();
  rasToLpsMatrix->SetElement(0,0,-1);
  rasToLpsMatrix->SetElement(1,1,-1);

  vtkMatrix4x4* ijkToLpsMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(ijkToRasMatrix, rasToLpsMatrix, ijkToLpsMatrix);

  for ( i=0; i<3; i++) {
    origin[i] =  ijkToRasMatrix->GetElement(3,i);
    int j;
    for (j=0; j<3; j++) {
      direction[j][i] =  ijkToLpsMatrix->GetElement(i,j);
    }
  }

  rasToLpsMatrix->Delete();
  ijkToRasMatrix->Delete();
  ijkToLpsMatrix->Delete();

  origin[0] *= -1;
  origin[1] *= -1;

  // itk import for input itk images
  typedef typename itk::VTKImageImport<ImageType> ImageImportType;
  typename ImageImportType::Pointer itkImporter = ImageImportType::New();

  // vtk export for  vtk image
  vtkImageExport* vtkExporter = vtkImageExport::New();  
  vtkImageFlip* vtkFlip = vtkImageFlip::New();

  // writer 
  typedef typename itk::ImageFileWriter<ImageType> ImageWriterType;      
  typename ImageWriterType::Pointer   itkImageWriter =  ImageWriterType::New();

  if ( self->GetUseCompression() ) 
    {
    itkImageWriter->UseCompressionOn();
    }
    else
    {
    itkImageWriter->UseCompressionOff();
    }


  // set pipeline for the image
  vtkFlip->SetInput( inputImage );
  vtkFlip->SetFilteredAxis(1);
  vtkFlip->FlipAboutOriginOn();

  vtkExporter->SetInput ( inputImage );

  itkImporter = ImageImportType::New();
  ConnectPipelines(vtkExporter, itkImporter);

  // write image
  itkImageWriter->SetInput(itkImporter->GetOutput());
  itkImporter->GetOutput()->SetDirection(direction);
  itkImporter->GetOutput()->Update();
  itkImporter->GetOutput()->SetOrigin(origin);
  itkImporter->GetOutput()->SetSpacing(mag);
  itkImageWriter->SetFileName( fileName );
  itkImageWriter->Update();

  // clean up
  vtkExporter->Delete();
  vtkFlip->Delete();
}
#endif

template <class  TPixelType>
void ITKWriteVTKImage(vtkvmtkITKImageWriter *self, vtkImageData *inputImage, char *fileName, 
                      vtkMatrix4x4* rasToIjkMatrix, TPixelType dummy) {

  typedef  itk::Image<TPixelType, 3> ImageType;

  double mag[3];
  typename ImageType::DirectionType direction;
  typename ImageType::PointType origin;
  direction.SetIdentity();
 
  if (rasToIjkMatrix)
    {
    vtkMatrix4x4 *ijkToRasMatrix = vtkMatrix4x4::New();
    vtkMatrix4x4::Invert(rasToIjkMatrix, ijkToRasMatrix);
  
    ijkToRasMatrix->Transpose();
  
    int i;
    for (i=0; i<3; i++) {
      // normalize vectors
      mag[i] = 0;
      for (int j=0; j<3; j++) {
        mag[i] += ijkToRasMatrix->GetElement(i,j)* ijkToRasMatrix->GetElement(i,j);
      }
      if (mag[i] == 0.0) {
        mag[i] = 1;
      }
      mag[i] = sqrt(mag[i]);
      //if (i == 1) { // Y flip
        //mag[i] = -mag[i];
      //}
    }
  
    for ( i=0; i<3; i++) {
      int j;
      for (j=0; j<3; j++) {
        ijkToRasMatrix->SetElement(i, j, ijkToRasMatrix->GetElement(i,j)/mag[i]);
      }
    }
    
    // ITK image direction are in LPS space
    // convert from ijkToRas to ijkToLps
    vtkMatrix4x4* rasToLpsMatrix = vtkMatrix4x4::New();
    rasToLpsMatrix->Identity();
    rasToLpsMatrix->SetElement(0,0,-1);
    rasToLpsMatrix->SetElement(1,1,-1);
  
    vtkMatrix4x4* ijkToLpsMatrix = vtkMatrix4x4::New();
    vtkMatrix4x4::Multiply4x4(ijkToRasMatrix, rasToLpsMatrix, ijkToLpsMatrix);
  
    for ( i=0; i<3; i++) {
      origin[i] =  ijkToRasMatrix->GetElement(3,i);
      int j;
      for (j=0; j<3; j++) {
        direction[j][i] =  ijkToLpsMatrix->GetElement(i,j);
      }
    }
  
    rasToLpsMatrix->Delete();
    ijkToRasMatrix->Delete();
    ijkToLpsMatrix->Delete();

    origin[0] *= -1;
    origin[1] *= -1;
    }
  else
    {
    // ITK image direction are in LPS space
    vtkMatrix4x4* rasToLpsMatrix = vtkMatrix4x4::New();
    rasToLpsMatrix->Identity();
    rasToLpsMatrix->SetElement(0,0,-1);
    rasToLpsMatrix->SetElement(1,1,-1);
    
    int i; 
    for ( i=0; i<3; i++) {
      int j;
      for (j=0; j<3; j++) {
        direction[j][i] =  rasToLpsMatrix->GetElement(i,j);
      }
    }
  
    rasToLpsMatrix->Delete();

    double inputOrigin[3];
    inputImage->GetOrigin(inputOrigin); 

    origin[0] = -1.0 * inputOrigin[0];
    origin[1] = -1.0 * inputOrigin[1];
    origin[2] = inputOrigin[2];
    }

  // itk import for input itk images
  typedef typename itk::VTKImageImport<ImageType> ImageImportType;
  typename ImageImportType::Pointer itkImporter = ImageImportType::New();

  // vtk export for  vtk image
  vtkImageExport* vtkExporter = vtkImageExport::New();  

  // writer 
  typedef typename itk::ImageFileWriter<ImageType> ImageWriterType;      
  typename ImageWriterType::Pointer   itkImageWriter =  ImageWriterType::New();

  if ( self->GetUseCompression() ) 
    {
    itkImageWriter->UseCompressionOn();
    }
    else
    {
    itkImageWriter->UseCompressionOff();
    }

  // set pipeline for the image
  vtkExporter->SetInput ( inputImage );

  itkImporter = ImageImportType::New();
  ConnectPipelines(vtkExporter, itkImporter);

  // write image
  itkImageWriter->SetInput(itkImporter->GetOutput());
  if (rasToIjkMatrix)
    {
    itkImporter->GetOutput()->SetDirection(direction);
    itkImporter->GetOutput()->Update();
    itkImporter->GetOutput()->SetOrigin(origin);
    itkImporter->GetOutput()->SetSpacing(mag);
    }
  else
    {
    itkImporter->GetOutput()->SetDirection(direction);
    itkImporter->GetOutput()->Update();
    itkImporter->GetOutput()->SetOrigin(origin);
    }
  itkImporter->Update();
  itkImageWriter->SetFileName( fileName );
  itkImageWriter->Update();

  // clean up
  vtkExporter->Delete();
}

//----------------------------------------------------------------------------
vtkvmtkITKImageWriter::vtkvmtkITKImageWriter()
{
  this->FileName = NULL;
  this->RasToIJKMatrix = NULL;
  this->UseCompression = 0;
}


//----------------------------------------------------------------------------
vtkvmtkITKImageWriter::~vtkvmtkITKImageWriter()
{
  // get rid of memory allocated for file names
  if (this->FileName) {
    delete [] this->FileName;
    this->FileName = NULL;
  }
}


//----------------------------------------------------------------------------
void vtkvmtkITKImageWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}


//----------------------------------------------------------------------------
void vtkvmtkITKImageWriter::SetInput(vtkImageData *input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
vtkImageData *vtkvmtkITKImageWriter::GetInput()
{
  if (this->NumberOfInputs < 1) {
    return NULL;
  }
  
  return (vtkImageData *)(this->Inputs[0]);
}



//----------------------------------------------------------------------------
// This function sets the name of the file. 
void vtkvmtkITKImageWriter::SetFileName(const char *name)
{
  if ( this->FileName && name && (!strcmp(this->FileName,name))) {
    return;
  }
  if (!name && !this->FileName) {
    return;
  }
  if (this->FileName) {
    delete [] this->FileName;
  }

  this->FileName = new char[strlen(name) + 1];
  strcpy(this->FileName, name);
  this->Modified();
}

//----------------------------------------------------------------------------
// Writes all the data from the input.
void vtkvmtkITKImageWriter::Write()
{
  if ( this->GetInput() == NULL ) {
    return;
  }
  if ( ! this->FileName ) {
    vtkErrorMacro(<<"vtkvmtkITKImageWriter: Please specify a FileName");
    return;
  }

  this->GetInput()->UpdateInformation();
  this->GetInput()->SetUpdateExtent(this->GetInput()->GetWholeExtent());

  if (this->GetInput()->GetNumberOfScalarComponents() == 1) {

    // take into consideration the scalar type
    switch (this->GetInput()->GetScalarType())
      {
      case VTK_DOUBLE:
        {
          double pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_FLOAT:
        {
          float pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_LONG:
        {
          long pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_UNSIGNED_LONG:
        {
          unsigned long pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_INT:
        {
          int pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_UNSIGNED_INT:
        {
          unsigned int pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_SHORT:
        {
          short pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType); 
        }
        break;
      case VTK_UNSIGNED_SHORT:
        {
          unsigned short pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_CHAR:
        {
          char pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_UNSIGNED_CHAR:
        {
          unsigned char pixelType=0;
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      default:
        vtkErrorMacro(<< "Execute: Unknown output ScalarType");
        return; 
      }
  } // scalar

  else if (this->GetInput()->GetNumberOfScalarComponents() == 3) {

    // take into consideration the scalar type
    switch (this->GetInput()->GetScalarType())
      {
      case VTK_DOUBLE:
        {
          typedef itk::Vector<double, 3>    VectorPixelType;
          VectorPixelType pixelType;
          pixelType.Fill(0.0);
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_FLOAT:
        {
          typedef itk::Vector<float, 3>    VectorPixelType;
          VectorPixelType pixelType;
          pixelType.Fill(0.0);
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_UNSIGNED_SHORT:
        {
          typedef itk::Vector<unsigned short, 3>    VectorPixelType;
          VectorPixelType pixelType;
          pixelType.Fill(0);
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      case VTK_UNSIGNED_CHAR:
        {
          typedef itk::Vector<unsigned char, 3>    VectorPixelType;
          VectorPixelType pixelType;
          pixelType.Fill(0);
          ITKWriteVTKImage(this, this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
        }
        break;
      default:
        vtkErrorMacro(<< "Execute: Unknown output ScalarType");
        return; 
      }
  } // vector
  else {
    vtkErrorMacro(<< "Can only export 1 or 3 component images");
    return; 
  }
}


