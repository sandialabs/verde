/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void MetricRangeDialog::init()
{
  Restore();
  SetValidator();
}

// this will restore the hard-coded defaults
void MetricRangeDialog::Reset()
{
  EdgeMetric::instance()->reset_range();
  QuadMetric::instance()->reset_range();
  PyramidMetric::instance()->reset_range();
  TriMetric::instance()->reset_range();
  HexMetric::instance()->reset_range();
  TetMetric::instance()->reset_range();
  WedgeMetric::instance()->reset_range();
  KnifeMetric::instance()->reset_range();
  Restore();  
}

void MetricRangeDialog::SetValidator()
{
  validator = new QDoubleValidator(this);
  edgeMinLengthVal->setValidator(validator);
  edgeMaxLengthVal->setValidator(validator);
  
  quadMinAspectVal->setValidator(validator);
  quadMinSkewVal->setValidator(validator);
  quadMinTaperVal->setValidator(validator);
  quadMinWarpageVal->setValidator(validator);
  quadMinAreaVal->setValidator(validator);
  quadMinStretchVal->setValidator(validator);
  quadMinAngle1Val->setValidator(validator);
  quadMinAngle2Val->setValidator(validator);
  quadMinOddyVal->setValidator(validator);
  quadMinConditionVal->setValidator(validator);
  quadMinJacobianVal->setValidator(validator);
  quadMinSJacobianVal->setValidator(validator);
  quadMinShearVal->setValidator(validator);
  quadMinShapeVal->setValidator(validator);
  quadMinRelSizeVal->setValidator(validator);
  quadMinShapeSizeVal->setValidator(validator);
  
  quadMaxAspectVal->setValidator(validator);
  quadMaxSkewVal->setValidator(validator);
  quadMaxTaperVal->setValidator(validator);
  quadMaxWarpageVal->setValidator(validator);
  quadMaxAreaVal->setValidator(validator);
  quadMaxStretchVal->setValidator(validator);
  quadMaxAngle1Val->setValidator(validator);
  quadMaxAngle2Val->setValidator(validator);
  quadMaxOddyVal->setValidator(validator);
  quadMaxConditionVal->setValidator(validator);
  quadMaxJacobianVal->setValidator(validator);
  quadMaxSJacobianVal->setValidator(validator);
  quadMaxShearVal->setValidator(validator);
  quadMaxShapeVal->setValidator(validator);
  quadMaxRelSizeVal->setValidator(validator);
  quadMaxShapeSizeVal->setValidator(validator);

  
  triMinAspectVal->setValidator(validator);
  triMinAreaVal->setValidator(validator);
  triMinAngle1Val->setValidator(validator);
  triMinAngle2Val->setValidator(validator);
  triMinConditionVal->setValidator(validator);
  triMinSJacobianVal->setValidator(validator);
  triMinShearVal->setValidator(validator);
  triMinShapeVal->setValidator(validator);
  triMinRelSizeVal->setValidator(validator);
  triMinShapeSizeVal->setValidator(validator);

  triMaxAspectVal->setValidator(validator);
  triMaxAreaVal->setValidator(validator);
  triMaxAngle1Val->setValidator(validator);
  triMaxAngle2Val->setValidator(validator);
  triMaxConditionVal->setValidator(validator);
  triMaxSJacobianVal->setValidator(validator);
  triMaxShearVal->setValidator(validator);
  triMaxShapeVal->setValidator(validator);
  triMaxRelSizeVal->setValidator(validator);
  triMaxShapeSizeVal->setValidator(validator);
   
  hexMinAspectVal->setValidator(validator);
  hexMinSkewVal->setValidator(validator);
  hexMinTaperVal->setValidator(validator);
  hexMinVolumeVal->setValidator(validator);
  hexMinStretchVal->setValidator(validator);
  hexMinDiagonalVal->setValidator(validator);
  hexMinDimensionVal->setValidator(validator);
  hexMinOddyVal->setValidator(validator);
  hexMinConditionVal->setValidator(validator);
  hexMinJacobianVal->setValidator(validator);
  hexMinSJacobianVal->setValidator(validator);
  hexMinShearVal->setValidator(validator);
  hexMinShapeVal->setValidator(validator);
  hexMinRelSizeVal->setValidator(validator);
  hexMinShapeSizeVal->setValidator(validator);

  hexMaxAspectVal->setValidator(validator);
  hexMaxSkewVal->setValidator(validator);
  hexMaxTaperVal->setValidator(validator);
  hexMaxVolumeVal->setValidator(validator);
  hexMaxStretchVal->setValidator(validator);
  hexMaxDiagonalVal->setValidator(validator);
  hexMaxDimensionVal->setValidator(validator);
  hexMaxOddyVal->setValidator(validator);
  hexMaxConditionVal->setValidator(validator);
  hexMaxJacobianVal->setValidator(validator);
  hexMaxSJacobianVal->setValidator(validator);
  hexMaxShearVal->setValidator(validator);
  hexMaxShapeVal->setValidator(validator);
  hexMaxRelSizeVal->setValidator(validator);
  hexMaxShapeSizeVal->setValidator(validator);
   
   
  tetMinAspectVal->setValidator(validator);
  tetMinAspectGammaVal->setValidator(validator);
  tetMinVolumeVal->setValidator(validator);
  tetMinConditionVal->setValidator(validator);
  tetMinJacobianVal->setValidator(validator);
  tetMinSJacobianVal->setValidator(validator);
  tetMinShearVal->setValidator(validator);
  tetMinShapeVal->setValidator(validator);
  tetMinRelSizeVal->setValidator(validator);
  tetMinShapeSizeVal->setValidator(validator);

  tetMaxAspectVal->setValidator(validator);
  tetMaxAspectGammaVal->setValidator(validator);
  tetMaxVolumeVal->setValidator(validator);
  tetMaxConditionVal->setValidator(validator);
  tetMaxJacobianVal->setValidator(validator);
  tetMaxSJacobianVal->setValidator(validator);
  tetMaxShearVal->setValidator(validator);
  tetMaxShapeVal->setValidator(validator);
  tetMaxRelSizeVal->setValidator(validator);
  tetMaxShapeSizeVal->setValidator(validator);
   
   
  wedgeMinVolumeVal->setValidator(validator);
  wedgeMaxVolumeVal->setValidator(validator);
     
  pyramidMinVolumeVal->setValidator(validator);
  pyramidMaxVolumeVal->setValidator(validator);
  
  knifeMinVolumeVal->setValidator(validator);
  knifeMaxVolumeVal->setValidator(validator);
  
}


void MetricRangeDialog::Restore()
{
  
  restore_edge_vals(); 
  restore_quad_vals(); 
  restore_tri_vals(); 
  restore_hex_vals(); 
  restore_tet_vals(); 
  restore_wedge_vals(); 
  restore_pyramid_vals(); 
  restore_knife_vals();
  
}


void MetricRangeDialog::Set()
{
  EdgeSet();
  QuadSet();
  TriSet();
  HexSet();
  TetSet();
  WedgeSet();
  PyramidSet();
  KnifeSet();    
  
}

void MetricRangeDialog::restore_edge_vals()
{
  // Get Edge Metrics
  double num=0;
  
  num = EdgeMetric::instance()->get_min(EDGE_LENGTH);
  edgeMinLengthVal->setText(QString("%1").arg(num));
  num = EdgeMetric::instance()->get_max(EDGE_LENGTH);
  edgeMaxLengthVal->setText(QString("%1").arg(num));
}

void MetricRangeDialog::restore_quad_vals()
{
  // Get Quad Metrics
  double num = 0;
  
  num = QuadMetric::instance()->get_min(QUAD_ASPECT);
  quadMinAspectVal->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_ASPECT);
  quadMaxAspectVal->setText(QString("%1").arg(num));
  
  num = QuadMetric::instance()->get_min(QUAD_SKEW); 
  quadMinSkewVal->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_SKEW); 
  quadMaxSkewVal->setText(QString("%1").arg(num));
  
  num = QuadMetric::instance()->get_min(QUAD_TAPER); 
  quadMinTaperVal->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_TAPER); 
  quadMaxTaperVal->setText(QString("%1").arg(num));
  
  num = QuadMetric::instance()->get_min(QUAD_WARPAGE); 
  quadMinWarpageVal->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_WARPAGE); 
  quadMaxWarpageVal->setText(QString("%1").arg(num));
  
  num = QuadMetric::instance()->get_min(QUAD_AREA); 
  quadMinAreaVal->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_AREA); 
  quadMaxAreaVal->setText(QString("%1").arg(num));
  
  num = QuadMetric::instance()->get_min(QUAD_STRETCH); 
  quadMinStretchVal->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_STRETCH); 
  quadMaxStretchVal->setText(QString("%1").arg(num));
  
  num = QuadMetric::instance()->get_min(QUAD_MIN_ANGLE); 
  quadMinAngle2Val->setText(QString("%1").arg(num));
  num = QuadMetric::instance()->get_max(QUAD_MIN_ANGLE); 
  quadMaxAngle2Val->setText(QString("%1").arg(num));
 
  num = QuadMetric::instance()->get_min(QUAD_ANGLE);  
  quadMinAngle1Val->setText(QString("%1").arg(num)); 
  num = QuadMetric::instance()->get_max(QUAD_ANGLE);  
  quadMaxAngle1Val->setText(QString("%1").arg(num)); 
  
  num = QuadMetric::instance()->get_min(QUAD_MAX_ODDY);  
  quadMinOddyVal->setText(QString("%1").arg(num)); 
  num = QuadMetric::instance()->get_max(QUAD_MAX_ODDY);  
  quadMaxOddyVal->setText(QString("%1").arg(num)); 

  num = QuadMetric::instance()->get_min(QUAD_MAX_COND);  
  quadMinConditionVal->setText(QString("%1").arg(num)); 
  num = QuadMetric::instance()->get_max(QUAD_MAX_COND);  
  quadMaxConditionVal->setText(QString("%1").arg(num)); 
  
  num = QuadMetric::instance()->get_min(QUAD_MIN_JAC);  
  quadMinJacobianVal->setText(QString("%1").arg(num)); 
  num = QuadMetric::instance()->get_max(QUAD_MIN_JAC);  
  quadMaxJacobianVal->setText(QString("%1").arg(num)); 

  num = QuadMetric::instance()->get_min(QUAD_MIN_SC_JAC);  
  quadMinSJacobianVal->setText(QString("%1").arg(num)); 
  num = QuadMetric::instance()->get_max(QUAD_MIN_SC_JAC);  
  quadMaxSJacobianVal->setText(QString("%1").arg(num)); 
  
  num = QuadMetric::instance()->get_min(QUAD_SHEAR);  
  quadMinShearVal->setText(QString("%1").arg(num)); 
  num = QuadMetric::instance()->get_max(QUAD_SHEAR);  
  quadMaxShearVal->setText(QString("%1").arg(num)); 

  num = QuadMetric::instance()->get_min(QUAD_SHAPE);   
  quadMinShapeVal->setText(QString("%1").arg(num));  
  num = QuadMetric::instance()->get_max(QUAD_SHAPE);   
  quadMaxShapeVal->setText(QString("%1").arg(num));  
  
  num = QuadMetric::instance()->get_min(QUAD_REL_SIZE);   
  quadMinRelSizeVal->setText(QString("%1").arg(num));  
  num = QuadMetric::instance()->get_max(QUAD_REL_SIZE);   
  quadMaxRelSizeVal->setText(QString("%1").arg(num));  
  
  num = QuadMetric::instance()->get_min(QUAD_SHAPE_SIZE);   
  quadMinShapeSizeVal->setText(QString("%1").arg(num));  
  num = QuadMetric::instance()->get_max(QUAD_SHAPE_SIZE);   
  quadMaxShapeSizeVal->setText(QString("%1").arg(num));  
  
}

void MetricRangeDialog::restore_tri_vals()
{
  // Get Tri Metrics    
  double num = 0;
  
  num = TriMetric::instance()->get_min(TRI_ASPECT);   
  triMinAspectVal->setText(QString("%1").arg(num));  
  num = TriMetric::instance()->get_max(TRI_ASPECT);   
  triMaxAspectVal->setText(QString("%1").arg(num));  
  
  num = TriMetric::instance()->get_min(TRI_AREA);
  triMinAreaVal->setText(QString("%1").arg(num));
  num = TriMetric::instance()->get_max(TRI_AREA);
  triMaxAreaVal->setText(QString("%1").arg(num));
  
  num = TriMetric::instance()->get_min(TRI_ANGLE);    
  triMinAngle1Val->setText(QString("%1").arg(num));   
  num = TriMetric::instance()->get_max(TRI_ANGLE);    
  triMaxAngle1Val->setText(QString("%1").arg(num));   
  
  num = TriMetric::instance()->get_min(TRI_MIN_ANGLE);    
  triMinAngle2Val->setText(QString("%1").arg(num));   
  num = TriMetric::instance()->get_max(TRI_MIN_ANGLE);    
  triMaxAngle2Val->setText(QString("%1").arg(num));   
  
  num = TriMetric::instance()->get_min(TRI_CONDITION_NUMBER); 
  triMinConditionVal->setText(QString("%1").arg(num)); 
  num = TriMetric::instance()->get_max(TRI_CONDITION_NUMBER); 
  triMaxConditionVal->setText(QString("%1").arg(num)); 

  num = TriMetric::instance()->get_min(TRI_MIN_SC_JAC); 
  triMinSJacobianVal->setText(QString("%1").arg(num)); 
  num = TriMetric::instance()->get_max(TRI_MIN_SC_JAC); 
  triMaxSJacobianVal->setText(QString("%1").arg(num)); 
 
  num = TriMetric::instance()->get_min(TRI_SHEAR); 
  triMinShearVal->setText(QString("%1").arg(num)); 
  num = TriMetric::instance()->get_max(TRI_SHEAR); 
  triMaxShearVal->setText(QString("%1").arg(num)); 
  
  num = TriMetric::instance()->get_min(TRI_SHAPE); 
  triMinShapeVal->setText(QString("%1").arg(num)); 
  num = TriMetric::instance()->get_max(TRI_SHAPE); 
  triMaxShapeVal->setText(QString("%1").arg(num)); 
  
  num = TriMetric::instance()->get_min(TRI_REL_SIZE); 
  triMinRelSizeVal->setText(QString("%1").arg(num)); 
  num = TriMetric::instance()->get_max(TRI_REL_SIZE); 
  triMaxRelSizeVal->setText(QString("%1").arg(num)); 
  
  num = TriMetric::instance()->get_min(TRI_SHAPE_SIZE);
  triMinShapeSizeVal->setText(QString("%1").arg(num));
  num = TriMetric::instance()->get_max(TRI_SHAPE_SIZE);
  triMaxShapeSizeVal->setText(QString("%1").arg(num));
    
}

void MetricRangeDialog::restore_hex_vals()
{
  // Get Hex Metrics
  double num = 0;
  
  num = HexMetric::instance()->get_min(HEX_ASPECT);
  hexMinAspectVal->setText(QString("%1").arg(num));
  num = HexMetric::instance()->get_max(HEX_ASPECT);
  hexMaxAspectVal->setText(QString("%1").arg(num));
    
  num = HexMetric::instance()->get_min(HEX_SKEW); 
  hexMinSkewVal->setText(QString("%1").arg(num)); 
  num = HexMetric::instance()->get_max(HEX_SKEW); 
  hexMaxSkewVal->setText(QString("%1").arg(num)); 
  
  num = HexMetric::instance()->get_min(HEX_TAPER);  
  hexMinTaperVal->setText(QString("%1").arg(num));  
  num = HexMetric::instance()->get_max(HEX_TAPER);  
  hexMaxTaperVal->setText(QString("%1").arg(num));  
  
  num = HexMetric::instance()->get_min(HEX_VOLUME);  
  hexMinVolumeVal->setText(QString("%1").arg(num));  
  num = HexMetric::instance()->get_max(HEX_VOLUME);  
  hexMaxVolumeVal->setText(QString("%1").arg(num));  
  
  num = HexMetric::instance()->get_min(HEX_STRETCH);  
  hexMinStretchVal->setText(QString("%1").arg(num));  
  num = HexMetric::instance()->get_max(HEX_STRETCH);  
  hexMaxStretchVal->setText(QString("%1").arg(num));  
  
  num = HexMetric::instance()->get_min(HEX_DIAGONALS);  
  hexMinDiagonalVal->setText(QString("%1").arg(num));  
  num = HexMetric::instance()->get_max(HEX_DIAGONALS);  
  hexMaxDiagonalVal->setText(QString("%1").arg(num));  
  
  num = HexMetric::instance()->get_min(HEX_CHARDIM);  
  hexMinDimensionVal->setText(QString("%1").arg(num));  
  num = HexMetric::instance()->get_max(HEX_CHARDIM);  
  hexMaxDimensionVal->setText(QString("%1").arg(num));  

  num = HexMetric::instance()->get_min(HEX_ODDY);   
  hexMinOddyVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_ODDY);   
  hexMaxOddyVal->setText(QString("%1").arg(num));   
   
  num = HexMetric::instance()->get_min(HEX_CONDITION);   
  hexMinConditionVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_CONDITION);   
  hexMaxConditionVal->setText(QString("%1").arg(num));   
   
  num = HexMetric::instance()->get_min(HEX_JACOBIAN);   
  hexMinJacobianVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_JACOBIAN);   
  hexMaxJacobianVal->setText(QString("%1").arg(num));   
  
  num = HexMetric::instance()->get_min(HEX_NORM_JACOBIAN);   
  hexMinSJacobianVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_NORM_JACOBIAN);   
  hexMaxSJacobianVal->setText(QString("%1").arg(num));   
   
  num = HexMetric::instance()->get_min(HEX_SHEAR);   
  hexMinShearVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_SHEAR);   
  hexMaxShearVal->setText(QString("%1").arg(num));   
   
  num = HexMetric::instance()->get_min(HEX_SHAPE);   
  hexMinShapeVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_SHAPE);   
  hexMaxShapeVal->setText(QString("%1").arg(num));   
   
  num = HexMetric::instance()->get_min(HEX_RELSIZE);   
  hexMinRelSizeVal->setText(QString("%1").arg(num));   
  num = HexMetric::instance()->get_max(HEX_RELSIZE);   
  hexMaxRelSizeVal->setText(QString("%1").arg(num));   
   
  num = HexMetric::instance()->get_min(HEX_SHAPE_SIZE );    
  hexMinShapeSizeVal->setText(QString("%1").arg(num));    
  num = HexMetric::instance()->get_max(HEX_SHAPE_SIZE);    
  hexMaxShapeSizeVal->setText(QString("%1").arg(num));  
 
}

void MetricRangeDialog::restore_tet_vals()
{
  // Get Tet Metrics
  double num = 0;
  
  num = TetMetric::instance()->get_min(ASPECT_RATIO);
  tetMinAspectVal->setText(QString("%1").arg(num));
  num = TetMetric::instance()->get_max(ASPECT_RATIO);
  tetMaxAspectVal->setText(QString("%1").arg(num));
  
  num = TetMetric::instance()->get_min(ASPECT_RATIO_GAMMA); 
  tetMinAspectGammaVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(ASPECT_RATIO_GAMMA); 
  tetMaxAspectGammaVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_VOLUME); 
  tetMinVolumeVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_VOLUME); 
  tetMaxVolumeVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_CONDITION_NUMBER); 
  tetMinConditionVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_CONDITION_NUMBER); 
  tetMaxConditionVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_JACOBIAN); 
  tetMinJacobianVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_JACOBIAN); 
  tetMaxJacobianVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_NORM_JACOBIAN); 
  tetMinSJacobianVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_NORM_JACOBIAN); 
  tetMaxSJacobianVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_SHEAR); 
  tetMinShearVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_SHEAR); 
  tetMaxShearVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_SHAPE); 
  tetMinShapeVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_SHAPE); 
  tetMaxShapeVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_RELSIZE); 
  tetMinRelSizeVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_RELSIZE); 
  tetMaxRelSizeVal->setText(QString("%1").arg(num)); 
   
  num = TetMetric::instance()->get_min(TET_SHAPE_SIZE); 
  tetMinShapeSizeVal->setText(QString("%1").arg(num)); 
  num = TetMetric::instance()->get_max(TET_SHAPE_SIZE); 
  tetMaxShapeSizeVal->setText(QString("%1").arg(num)); 
     
}

void MetricRangeDialog::restore_wedge_vals()
{
  // Get Wedge Metrics
  
  double num = 0;

  num = WedgeMetric::instance()->get_min(WEDGE_VOLUME);  
  wedgeMinVolumeVal->setText(QString("%1").arg(num));  
  num = WedgeMetric::instance()->get_max(WEDGE_VOLUME);  
  wedgeMaxVolumeVal->setText(QString("%1").arg(num));
  
}

void MetricRangeDialog::restore_pyramid_vals()
{
  // Get Pyramid Metrics
  
  double num = 0; 

  num = PyramidMetric::instance()->get_min(PYRAMID_VOLUME);   
  pyramidMinVolumeVal->setText(QString("%1").arg(num));   
  num = PyramidMetric::instance()->get_max(PYRAMID_VOLUME);   
  pyramidMaxVolumeVal->setText(QString("%1").arg(num)); 
 
}

void MetricRangeDialog::restore_knife_vals()
{
  // Get Knife Metrics
  
  double num = 0; 

  num = KnifeMetric::instance()->get_min(KNIFE_VOLUME);   
  knifeMinVolumeVal->setText(QString("%1").arg(num));   
  num = KnifeMetric::instance()->get_max(KNIFE_VOLUME);   
  knifeMaxVolumeVal->setText(QString("%1").arg(num)); 
   
}

void MetricRangeDialog::EdgeSet()
{
  QString num_str = edgeMinLengthVal->text();
  bool ok = 0;
  //char command[100];
  QString command;
  double num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set edge length minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
      
  num_str = edgeMaxLengthVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set edge length maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
}

void MetricRangeDialog::QuadSet()
{
  bool ok = 0;
  QString command;
  QString num_str = quadMinAspectVal->text();
  double num = num_str.toDouble(&ok);
  if (ok)  
  {
    command = "set quad aspect minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = quadMaxAspectVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set quad aspect maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = quadMinSkewVal->text(); 
  num = num_str.toDouble(&ok); 
  if (ok) 
  {  
    command = "set quad skew minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);  
  }  
  
  num_str = quadMaxSkewVal->text(); 
  num = num_str.toDouble(&ok); 
  if (ok) 
  {  
    command = "set quad skew maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);  
  }  
  
  num_str = quadMinTaperVal->text();  
  num = num_str.toDouble(&ok);  
  if (ok) 
  {   
    command = "set quad taper minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);   
  }   
  
  num_str = quadMaxTaperVal->text();  
  num = num_str.toDouble(&ok);  
  if (ok) 
  {   
    command = "set quad taper maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);   
  }   
  
  
  num_str = quadMinWarpageVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad warpage minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxWarpageVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad warpage maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinAreaVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad area minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxAreaVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad area maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    

  num_str = quadMinStretchVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad stretch minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxStretchVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad stretch maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinAngle1Val->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command =  "set quad largest angle minimum " +  num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxAngle1Val->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command =  "set quad largest angle maximum " +  num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinAngle2Val->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command =  "set quad smallest angle minimum " +  num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxAngle2Val->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command =  "set quad smallest angle maximum " +  num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    

  num_str = quadMinOddyVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command =  "set quad oddy minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxOddyVal->text();   
  num = num_str.toDouble(&ok); 
  if (ok) 
  {    
    command =  "set quad oddy maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinConditionVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad condition minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxConditionVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad condition maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    

  num_str = quadMinJacobianVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad jacobian minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxJacobianVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad jacobian maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinSJacobianVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad scaled jacobian minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxSJacobianVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad scaled jacobian maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinShearVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad shear minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxShearVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad shear maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinShapeVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  { 
    command = "set quad shape minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxShapeVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad shape maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  
  num_str = quadMinRelSizeVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad relative size minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxRelSizeVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad relative size maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    

  num_str = quadMinShapeSizeVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad shape and size minimum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
   
  num_str = quadMaxShapeSizeVal->text();   
  num = num_str.toDouble(&ok);   
  if (ok) 
  {    
    command = "set quad shape and size maximum " + num_str;    
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);    
  }    
  

}

void MetricRangeDialog::TriSet()
{
  bool ok = 0;
  QString command;
  QString num_str = triMinAspectVal->text();
  double num = num_str.toDouble(&ok);
  if (ok)  
  {
    command = "set tri aspect minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
      
  num_str = triMaxAspectVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri aspect maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinAreaVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri area minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMaxAreaVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri area maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinAngle1Val->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri largest angle minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxAngle1Val->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri largest angle maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 

  num_str = triMinAngle2Val->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri smallest angle minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxAngle2Val->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri smallest angle maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinConditionVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri condition minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxConditionVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri condition maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinSJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri scaled jacobian minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxSJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri scaled jacobian maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinShearVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri shear minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxShearVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri shear maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMinShapeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri shape minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxShapeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri shape maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinRelSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri relative size minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxRelSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri relative size maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
  num_str = triMinShapeSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri shape and size minimum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  num_str = triMaxShapeSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set tri shape and size maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 
  
}

void MetricRangeDialog::HexSet()
{
  bool ok = 0;
  QString command;
  QString num_str = hexMinAspectVal->text();
  double num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set hex aspect minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxAspectVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex aspect maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinSkewVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex skew minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxSkewVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex skew maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinTaperVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex taper minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxTaperVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex taper maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
 
  num_str = hexMinVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex volume minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex volume maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMinStretchVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex stretch minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxStretchVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex stretch maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinDiagonalVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex diagonal minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxDiagonalVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex diagonal maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinDimensionVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex dimension minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxDimensionVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex dimension maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinOddyVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex oddy minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxOddyVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex oddy maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinConditionVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex condition minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxConditionVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex condition maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex jacobian minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex jacobian maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinSJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex scaled jacobian minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxSJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex scaled jacobian maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinShearVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex shear minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxShearVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex shear maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinShapeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex shape minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxShapeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex shape maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinRelSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex relative size minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxRelSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex relative size maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = hexMinShapeSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex shape and size minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = hexMaxShapeSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set hex shape and size maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

}

void MetricRangeDialog::TetSet()
{
  bool ok = 0;
  QString command;
  QString num_str = tetMinAspectVal->text();
  double num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet aspect minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxAspectVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet aspect maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = tetMinAspectGammaVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet aspect gamma minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxAspectGammaVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet aspect gamma maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
  
  num_str = tetMinVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet volume minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet volume maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinConditionVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet condition minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxConditionVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet condition maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet jacobian minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet jacobian maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinSJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet scaled jacobian minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxJacobianVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet scaled jacobian maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinShearVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet shear minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxShearVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet shear maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinShapeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet shape minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxShapeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet shape maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinRelSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet relative size minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxRelSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet relative size maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    
  
  num_str = tetMinShapeSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set tet shape and size minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }

  num_str = tetMaxShapeSizeVal->text();
  num = num_str.toDouble(&ok);
  if (ok)
  {
    command = "set tet shape and size maximum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }    

  
  
  
}

void MetricRangeDialog::WedgeSet()
{
  bool ok = 0;
  QString command;
  QString num_str = wedgeMinVolumeVal->text();
  double num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set wedge volume minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
      
  num_str = wedgeMaxVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set wedge volume maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 


}

void MetricRangeDialog::PyramidSet()
{
  bool ok = 0;
  QString command;
  QString num_str = pyramidMinVolumeVal->text();
  double num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set pyramid volume minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
      
  num_str = pyramidMaxVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set pyramid volume maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 


  
}

void MetricRangeDialog::KnifeSet()
{
  bool ok = 0;
  QString command;
  QString num_str = knifeMinVolumeVal->text();
  double num = num_str.toDouble(&ok);
  if (ok) 
  {
    command = "set knife volume minimum " + num_str;
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE);
  }
      
  num_str = knifeMaxVolumeVal->text();
  num = num_str.toDouble(&ok);
  if (ok) 
  { 
    command = "set knife volume maximum " + num_str; 
    GUICommandHandler::instance()->add_command_string(command.latin1(), VERDE_FALSE); 
  } 


}


