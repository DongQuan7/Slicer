/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.13 $

=========================================================================auto=*/

#ifndef __vtkMRMLVolumeNode_h
#define __vtkMRMLVolumeNode_h

// MRML includes
#include "vtkMRMLDisplayableNode.h"
class vtkMRMLVolumeDisplayNode;

// VTK includes
class vtkAlgorithmOutput;
class vtkEventForwarderCommand;
class vtkImageData;
class vtkMatrix4x4;

// ITK includes
#include "itkMetaDataDictionary.h"

/// \brief MRML node for representing a volume (image stack).
///
/// Volume nodes describe data sets that can be thought of as stacks of 2D
/// images that form a 3D volume.  Volume nodes describe where the images
/// are stored on disk, how to render the data (window and level), and how
/// to read the files.  This information is extracted from the image
/// headers (if they exist) at the time the MRML file is generated.
/// Consequently, MRML files isolate MRML browsers from understanding how
/// to read the myriad of file formats for medical data.
class VTK_MRML_EXPORT vtkMRMLVolumeNode : public vtkMRMLDisplayableNode
{
public:
  vtkTypeMacro(vtkMRMLVolumeNode,vtkMRMLDisplayableNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE = 0;

  ///
  /// Read node attributes from XML file
  virtual void ReadXMLAttributes( const char** atts) VTK_OVERRIDE;

  ///
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent) VTK_OVERRIDE;

  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node) VTK_OVERRIDE;

  ///
  /// Copy the node's attributes to this object
  void CopyOrientation(vtkMRMLVolumeNode *node);


  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE = 0;

  ///
  /// Finds the storage node and read the data
  virtual void UpdateScene(vtkMRMLScene *scene) VTK_OVERRIDE;

  //--------------------------------------------------------------------------
  /// RAS->IJK Matrix Calculation
  //--------------------------------------------------------------------------

  ///
  /// The order of slices in the volume. One of: LR (left-to-right),
  /// RL, AP, PA, IS, SI. This information is encoded in the rasToIJKMatrix.
  /// This matrix can be computed either from corner points, or just he
  /// scanOrder.
  /// Return true on success, false otherwise
  static bool ComputeIJKToRASFromScanOrder(const char *order,
                                           const double* spacing,
                                           const int *dims,
                                           bool centerImage,
                                           vtkMatrix4x4 *IJKToRAS);

  static const char* ComputeScanOrderFromIJKToRAS(vtkMatrix4x4 *IJKToRAS);

  void SetIJKToRASDirections(double dirs[3][3]);
  void SetIJKToRASDirections(double ir, double ia, double is,
                             double jr, double ja, double js,
                             double kr, double ka, double ks);
  void SetIToRASDirection(double ir, double ia, double is);
  void SetJToRASDirection(double jr, double ja, double js);
  void SetKToRASDirection(double kr, double ka, double ks);

  void GetIJKToRASDirections(double dirs[3][3]);
  void GetIToRASDirection(double dirs[3]);
  void GetJToRASDirection(double dirs[3]);
  void GetKToRASDirection(double dirs[3]);

  ///
  /// Spacing and Origin, with the Directions, are the independent
  /// parameters that go to make up the IJKToRAS matrix
  /// In setter methods, StorableModifiedTime may need to be updated,
  /// which cannot be achieved by using vtkGetVector3Macro.
  vtkGetVector3Macro (Spacing, double);
  virtual void SetSpacing(double arg1, double arg2, double arg3);
  virtual void SetSpacing(double arg[3]);
  vtkGetVector3Macro (Origin, double);
  virtual void SetOrigin(double arg1, double arg2, double arg3);
  virtual void SetOrigin(double arg[3]);

  ///
  /// Utility function that returns the min spacing between the 3 orientations
  double GetMinSpacing();

  ///
  /// Utility function that returns the max spacing between the 3 orientations
  double GetMaxSpacing();

  ///
  /// Get the IJKToRAS Matrix that includes the spacing and origin
  /// information (assumes the image data is Origin 0 0 0 and Spacing 1 1 1)
  /// RASToIJK is the inverse of this
  void GetIJKToRASMatrix(vtkMatrix4x4* mat);
  void GetRASToIJKMatrix(vtkMatrix4x4* mat);

  void GetIJKToRASDirectionMatrix(vtkMatrix4x4* mat);
  void SetIJKToRASDirectionMatrix(vtkMatrix4x4* mat);

  ///
  /// Convenience methods to set the directions, spacing, and origin
  /// from a matrix
  void SetIJKToRASMatrix(vtkMatrix4x4* mat);
  void SetRASToIJKMatrix(vtkMatrix4x4* mat);

  ///
  /// Get bounding box in global RAS form (xmin,xmax, ymin,ymax, zmin,zmax).
  /// This method returns the bounds of the object with any transforms that may
  /// be applied to it.
  /// \sa GetSliceBounds(), GetIJKToRASMatrix(), vtkMRMLSliceLogic::GetVolumeSliceBounds()
  /// \sa GetNodeBounds()
  virtual void GetRASBounds(double bounds[6]) VTK_OVERRIDE;

  /// Get bounding box in global RAS form (xmin,xmax, ymin,ymax, zmin,zmax).
  /// This method always returns the bounds of the untransformed object.
  /// \sa GetRASBounds()
  virtual void GetBounds(double bounds[6]) VTK_OVERRIDE;

  ///
  /// Get bounding box in slice form (xmin,xmax, ymin,ymax, zmin,zmax).
  /// If not rasToSlice is passed, then it returns the bounds in global RAS form.
  /// \sa GetRASBounds()
  void GetSliceBounds(double bounds[6], vtkMatrix4x4* rasToSlice);

  ///
  /// Associated display MRML node
  virtual vtkMRMLVolumeDisplayNode* GetVolumeDisplayNode();

  /// In the ImageData object origin must be set to (0,0,0) and spacing must be set
  /// to (1,1,1). If the variables are set to different values then the application's
  /// behavior is undefined.
  /// The reason for not using origin and spacing in vtkImageData is that vtkImageData
  /// cannot store image orientation, and so it cannot store all the information that
  /// is necessary to compute the mapping between voxel (IJK) and  physical (RAS) coordinate systems.
  /// Instead of storing some information in vtkImageData and some outside, the decision was
  /// made to store all information in the MRML node (vtkMRMLVolumeNode::Origin,
  /// vtkMRMLVolumeNode::Spacing, and vtkMRMLVolumeNode::IJKToRASDirections).
  /// \sa GetImageData(), SetImageDataConnection()
  virtual void SetAndObserveImageData(vtkImageData *ImageData);
  virtual vtkImageData* GetImageData();
  /// Set and observe image data pipeline.
  /// It is propagated to the display nodes.
  /// \sa GetImageDataConnection()
  virtual void SetImageDataConnection(vtkAlgorithmOutput *inputPort);
  /// Return the input image data pipeline.
  vtkGetObjectMacro(ImageDataConnection, vtkAlgorithmOutput);

  ///
  /// Make sure image data of a volume node has extents that start at zero.
  /// This needs to be done for compatibility reasons, as many components assume the extent has a form of
  /// (0,dim[0],0,dim[1],0,dim[2]), which is not the case many times for segmentation merged labelmaps.
  void ShiftImageDataExtentToZeroStart();

  ///
  /// alternative method to propagate events generated in Display nodes
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/,
                                   unsigned long /*event*/,
                                   void * /*callData*/ ) VTK_OVERRIDE;

  /// ImageDataModifiedEvent is generated when image data is changed
  enum
    {
    ImageDataModifiedEvent = 18001
    };

  ///
  /// Set/Get the ITK MetaDataDictionary
  void SetMetaDataDictionary( const itk::MetaDataDictionary& );
  const itk::MetaDataDictionary& GetMetaDataDictionary() const;

  virtual bool CanApplyNonLinearTransforms()const VTK_OVERRIDE;

  virtual void ApplyTransform(vtkAbstractTransform* transform) VTK_OVERRIDE;

  virtual void ApplyTransformMatrix(vtkMatrix4x4* transformMatrix) VTK_OVERRIDE;

  virtual void ApplyNonLinearTransform(vtkAbstractTransform* transform);

  virtual bool GetModifiedSinceRead() VTK_OVERRIDE;

  ///
  /// Get background voxel value of the image. It can be used for assigning
  /// intensity value to "empty" voxels when the image is transformed.
  /// It is computed as median value of the 8 corner voxels.
  virtual double GetImageBackgroundScalarComponentAsDouble(int component);

protected:
  vtkMRMLVolumeNode();
  ~vtkMRMLVolumeNode();
  vtkMRMLVolumeNode(const vtkMRMLVolumeNode&);
  void operator=(const vtkMRMLVolumeNode&);

  /// Set the image data pipeline to all the display nodes.
  void SetImageDataToDisplayNodes();
  void SetImageDataToDisplayNode(vtkMRMLVolumeDisplayNode* displayNode);

  /// Called when a display node is added/removed/modified. Propagate the polydata
  /// to the new display node.
  virtual void UpdateDisplayNodeImageData(vtkMRMLDisplayNode *dnode);

  ///
  /// Called when a node reference ID is added (list size increased).
  virtual void OnNodeReferenceAdded(vtkMRMLNodeReference *reference) VTK_OVERRIDE;

  ///
  /// Called when a node reference ID is modified.
  virtual void OnNodeReferenceModified(vtkMRMLNodeReference *reference) VTK_OVERRIDE;

  ///
  /// Return the bounds of the node transformed or not depending on
  /// the useTransform parameter and the rasToSlice transform
  virtual void GetBoundsInternal(double bounds[6], vtkMatrix4x4* rasToSlice, bool useTransform);

  /// these are unit length direction cosines
  double IJKToRASDirections[3][3];

  /// these are mappings to mm space
  double Spacing[3];
  double Origin[3];

  vtkAlgorithmOutput* ImageDataConnection;
  vtkEventForwarderCommand* DataEventForwarder;

  itk::MetaDataDictionary Dictionary;
};

#endif
