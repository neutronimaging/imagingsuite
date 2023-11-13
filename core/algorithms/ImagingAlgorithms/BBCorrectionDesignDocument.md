# Design of new BB correction

## Requirements based in original design

### Image loading
- Load list of images
- Load ROI
- Averaging
  - Mean
  - Weighted average
  - Median
- Subtract dark

### Mask generation
- Segment and locate dots
- ROI handling
- Input data 
  - OB image
  - BB image 
- Provide COG list
- Provide mask image

### Estimation of scatter image
-  Estimate models
  - 2D Polynomial     
  - Thinn plate splines 
- Multiple models 
  - Interpolation between instances
  - Set mapping interval to normalized scan length 
- Get dose from scatter images

### Correction
- Normalization models
  - Standard
    - normalization   
    - log-norm

## New requirements

### Mask generation
- Segment stripes

### Estimation


### Python bindings
