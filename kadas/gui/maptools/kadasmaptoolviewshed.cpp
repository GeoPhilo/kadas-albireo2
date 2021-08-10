/***************************************************************************
    kadasmaptoolviewshed.cpp
    ------------------------
    copyright            : (C) 2019 by Sandro Mani
    email                : smani at sourcepole dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>

#include <qgis/qgsmapcanvas.h>
#include <qgis/qgsmultisurface.h>
#include <qgis/qgspolygon.h>
#include <qgis/qgsproject.h>
#include <qgis/qgsrasterlayer.h>
#include <qgis/qgsrastershader.h>
#include <qgis/qgssettings.h>
#include <qgis/qgssinglebandpseudocolorrenderer.h>

#include <kadas/core/kadas.h>
#include <kadas/core/kadascoordinateformat.h>
#include <kadas/analysis/kadasviewshedfilter.h>
#include <kadas/gui/mapitems/kadascircularsectoritem.h>
#include <kadas/gui/mapitems/kadassymbolitem.h>
#include <kadas/gui/maptools/kadasmaptoolviewshed.h>
#include <kadas/gui/kadasmapcanvasitemmanager.h>


KadasViewshedDialog::KadasViewshedDialog( double radius, QWidget *parent )
  : QDialog( parent )
{
  setWindowTitle( tr( "Viewshed setup" ) );

  QgsUnitTypes::DistanceUnit vertDisplayUnit = KadasCoordinateFormat::instance()->getHeightDisplayUnit();

  QGridLayout *heightDialogLayout = new QGridLayout();

  heightDialogLayout->addWidget( new QLabel( tr( "Observer height:" ) ), 0, 0, 1, 1 );
  mSpinBoxObserverHeight = new QDoubleSpinBox();
  mSpinBoxObserverHeight->setRange( 0, 8000 );
  mSpinBoxObserverHeight->setDecimals( 1 );
  mSpinBoxObserverHeight->setValue( 2. );
  mSpinBoxObserverHeight->setSuffix( vertDisplayUnit == QgsUnitTypes::DistanceFeet ? " ft" : " m" );
  heightDialogLayout->addWidget( mSpinBoxObserverHeight, 0, 1, 1, 1 );

  mComboObserverHeightMode = new QComboBox();
  mComboObserverHeightMode->addItem( tr( "Ground" ), static_cast<int>( HeightRelToGround ) );
  mComboObserverHeightMode->addItem( tr( "Sea level" ), static_cast<int>( HeightRelToSeaLevel ) );
  heightDialogLayout->addWidget( mComboObserverHeightMode, 0, 2, 1, 1 );

  heightDialogLayout->addWidget( new QLabel( tr( "Observer angle range:" ) ), 1, 0, 1, 1 );
  mSpinBoxObserverMinAngle = new QSpinBox();
  mSpinBoxObserverMinAngle->setRange( -90, 90 );
  mSpinBoxObserverMinAngle->setValue( -90 );
  mSpinBoxObserverMinAngle->setSuffix( "°" );
  mSpinBoxObserverMaxAngle = new QSpinBox();
  mSpinBoxObserverMaxAngle->setRange( -90, 90 );
  mSpinBoxObserverMaxAngle->setValue( 90 );
  mSpinBoxObserverMaxAngle->setSuffix( "°" );
  heightDialogLayout->addWidget( mSpinBoxObserverMinAngle, 1, 1, 1, 1 );
  heightDialogLayout->addWidget( mSpinBoxObserverMaxAngle, 1, 2, 1, 1 );
  connect( mSpinBoxObserverMinAngle, qOverload<int>( &QSpinBox::valueChanged ), this, &KadasViewshedDialog::adjustMaxAngle );
  connect( mSpinBoxObserverMaxAngle, qOverload<int>( &QSpinBox::valueChanged ), this, &KadasViewshedDialog::adjustMinAngle );

  heightDialogLayout->addWidget( new QLabel( tr( "Target height:" ) ), 2, 0, 1, 1 );
  mSpinBoxTargetHeight = new QDoubleSpinBox();
  mSpinBoxTargetHeight->setRange( 0, 999999999999 );
  mSpinBoxTargetHeight->setDecimals( 1 );
  mSpinBoxTargetHeight->setValue( 2. );
  mSpinBoxTargetHeight->setSuffix( vertDisplayUnit == QgsUnitTypes::DistanceFeet ? " ft" : " m" );
  heightDialogLayout->addWidget( mSpinBoxTargetHeight, 2, 1, 1, 1 );

  mComboTargetHeightMode = new QComboBox();
  mComboTargetHeightMode->addItem( tr( "Ground" ), static_cast<int>( HeightRelToGround ) );
  mComboTargetHeightMode->addItem( tr( "Sea level" ), static_cast<int>( HeightRelToSeaLevel ) );
  heightDialogLayout->addWidget( mComboTargetHeightMode, 2, 2, 1, 1 );

  heightDialogLayout->addWidget( new QLabel( tr( "Radius:" ) ), 3, 0, 1, 1 );
  QDoubleSpinBox *spinRadius = new QDoubleSpinBox();
  spinRadius->setRange( 1, 1000000000 );
  spinRadius->setDecimals( 0 );
  spinRadius->setValue( radius );
  spinRadius->setSuffix( " m" );
  spinRadius->setKeyboardTracking( false );
  connect( spinRadius, qOverload<double> ( &QDoubleSpinBox::valueChanged ), this, &KadasViewshedDialog::radiusChanged );
  heightDialogLayout->addWidget( spinRadius, 3, 1, 1, 2 );

  heightDialogLayout->addWidget( new QLabel( tr( "Display:" ) ), 4, 0, 1, 1 );
  mDisplayModeCombo = new QComboBox();
  mDisplayModeCombo->addItem( tr( "Visible area" ) );
  mDisplayModeCombo->addItem( tr( "Invisible area" ) );
  heightDialogLayout->addWidget( mDisplayModeCombo, 4, 1, 1, 2 );

  heightDialogLayout->addWidget( new QLabel( tr( "Accuracy:" ) ), 5, 0, 1, 1 );
  mAccuracySlider = new QSlider( Qt::Horizontal );
  mAccuracySlider->setRange( 1, 10 );
  mAccuracySlider->setTickPosition( QSlider::TicksBelow );
  mAccuracySlider->setTickInterval( 1 );
  heightDialogLayout->addWidget( mAccuracySlider, 5, 1, 1, 2 );

  QWidget *labelWidget = new QWidget( this );
  labelWidget->setLayout( new QHBoxLayout );
  labelWidget->layout()->setContentsMargins( 0, 0, 0, 0 );
  labelWidget->layout()->addWidget( new QLabel( QString( "<small>%1</small>" ).arg( tr( "Accurate" ) ) ) );
  labelWidget->layout()->addItem( new QSpacerItem( 1, 1, QSizePolicy::Expanding ) );
  labelWidget->layout()->addWidget( new QLabel( QString( "<small>%1</small>" ).arg( tr( "Fast" ) ) ) );
  heightDialogLayout->addWidget( labelWidget, 6, 1, 1, 2 );

  QDialogButtonBox *bbox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal );
  connect( bbox, &QDialogButtonBox::accepted, this, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, this, &QDialog::reject );
  heightDialogLayout->addWidget( bbox, 7, 0, 1, 3 );

  setLayout( heightDialogLayout );
  setFixedSize( sizeHint() );
}

double KadasViewshedDialog::observerHeight() const
{
  return mSpinBoxObserverHeight->value();
}

double KadasViewshedDialog::targetHeight() const
{
  return mSpinBoxTargetHeight->value();
}

bool KadasViewshedDialog::observerHeightRelativeToGround() const
{
  return static_cast<HeightMode>( mComboObserverHeightMode->itemData( mComboObserverHeightMode->currentIndex() ).toInt() ) == HeightRelToGround;
}

bool KadasViewshedDialog::targetHeightRelativeToGround() const
{
  return static_cast<HeightMode>( mComboTargetHeightMode->itemData( mComboTargetHeightMode->currentIndex() ).toInt() ) == HeightRelToGround;
}

double KadasViewshedDialog::observerMinVertAngle() const
{
  return mSpinBoxObserverMinAngle->value();
}

double KadasViewshedDialog::observerMaxVertAngle() const
{
  return mSpinBoxObserverMaxAngle->value();
}

KadasViewshedDialog::DisplayMode KadasViewshedDialog::displayMode() const
{
  return static_cast<DisplayMode>( mDisplayModeCombo->currentIndex() );
}

int KadasViewshedDialog::accuracyFactor() const
{
  return mAccuracySlider->value();
}

void KadasViewshedDialog::adjustMaxAngle()
{
  if ( mSpinBoxObserverMinAngle->value() >= mSpinBoxObserverMaxAngle->value() )
  {
    mSpinBoxObserverMaxAngle->setValue( std::min( 90, mSpinBoxObserverMinAngle->value() + 1 ) );
  }
}

void KadasViewshedDialog::adjustMinAngle()
{
  if ( mSpinBoxObserverMaxAngle->value() <= mSpinBoxObserverMinAngle->value() )
  {
    mSpinBoxObserverMinAngle->setValue( std::max( -90, mSpinBoxObserverMaxAngle->value() - 1 ) );
  }
}

///////////////////////////////////////////////////////////////////////////////

KadasMapToolViewshed::KadasMapToolViewshed( QgsMapCanvas *mapCanvas )
  : KadasMapToolCreateItem( mapCanvas, itemFactory( mapCanvas ) )
{
  setCursor( Qt::ArrowCursor );
  setToolLabel( tr( "Compute viewshed" ) );
  connect( this, &KadasMapToolCreateItem::partFinished, this, &KadasMapToolViewshed::drawFinished );
}

KadasMapToolCreateItem::ItemFactory KadasMapToolViewshed::itemFactory( const QgsMapCanvas *canvas ) const
{
  return [ = ]
  {
    KadasCircularSectorItem *item = new KadasCircularSectorItem( canvas->mapSettings().destinationCrs() );
    return item;
  };
}

void KadasMapToolViewshed::drawFinished()
{
  QString layerid = QgsProject::instance()->readEntry( "Heightmap", "layer" );
  QgsMapLayer *layer = QgsProject::instance()->mapLayer( layerid );
  if ( !layer || layer->type() != QgsMapLayerType::RasterLayer )
  {
    emit messageEmitted( tr( "No heightmap is defined in the project. Right-click a raster layer in the layer tree and select it to be used as heightmap." ), Qgis::Warning );
    clear();
    return;
  }

  const KadasCircularSectorItem *item = dynamic_cast<const KadasCircularSectorItem *>( currentItem() );
  if ( !item )
  {
    clear();
    return;
  }

  QgsPointXY center = item->constState()->centers.last();
  double curRadius = item->constState()->radii.last();

  QgsCoordinateReferenceSystem canvasCrs = canvas()->mapSettings().destinationCrs();
  curRadius *= QgsUnitTypes::fromUnitToUnitFactor( canvasCrs.mapUnits(), QgsUnitTypes::DistanceMeters );

  KadasViewshedDialog viewshedDialog( curRadius );
  connect( &viewshedDialog, &KadasViewshedDialog::radiusChanged, this, &KadasMapToolViewshed::adjustRadius );
  if ( viewshedDialog.exec() == QDialog::Rejected )
  {
    clear();
    return;
  }

  QString outputFileName = QString( "viewshed_%1,%2.tif" ).arg( center.x() ).arg( center.y() );
  QString outputFile = QgsProject::instance()->createAttachedFile( outputFileName );

  QVector<QgsPointXY> filterRegion;
  QgsPolygonXY poly = QgsGeometry( item->geometry()->geometryN( 0 )->clone() ).asPolygon();
  if ( !poly.isEmpty() )
  {
    filterRegion = poly.front();
  }
  center = item->constState()->centers.last();
  curRadius = item->constState()->radii.last();

  if ( mCanvas->mapSettings().mapUnits() == QgsUnitTypes::DistanceDegrees )
  {
    // Need to compute radius in meters
    QgsDistanceArea da;
    da.setEllipsoid( QgsProject::instance()->readEntry( "Measure", "/Ellipsoid", "NONE" ) );
    curRadius = da.measureLine( center, QgsPoint( center.x() + curRadius, center.y() ) );
    curRadius = da.convertLengthMeasurement( curRadius, QgsUnitTypes::DistanceMeters );
  }

  double heightConv = QgsUnitTypes::fromUnitToUnitFactor( KadasCoordinateFormat::instance()->getHeightDisplayUnit(), QgsUnitTypes::DistanceMeters );

  QProgressDialog p( tr( "Calculating viewshed..." ), tr( "Abort" ), 0, 0 );
  p.setWindowTitle( tr( "Viewshed" ) );
  p.setWindowModality( Qt::ApplicationModal );
  bool displayVisible = viewshedDialog.displayMode() == KadasViewshedDialog::DisplayVisibleArea;
  int accuracyFactor = viewshedDialog.accuracyFactor();
  QApplication::setOverrideCursor( Qt::WaitCursor );


  QString errMsg;
  bool success = KadasViewshedFilter::computeViewshed( static_cast<QgsRasterLayer *>( layer ), outputFile, "GTiff", center, canvasCrs, viewshedDialog.observerHeight() * heightConv, viewshedDialog.targetHeight() * heightConv, viewshedDialog.observerHeightRelativeToGround(), viewshedDialog.targetHeightRelativeToGround(), viewshedDialog.observerMinVertAngle(), viewshedDialog.observerMaxVertAngle(), curRadius, QgsUnitTypes::DistanceMeters, &p, &errMsg, filterRegion, displayVisible, accuracyFactor );
  QApplication::restoreOverrideCursor();
  if ( success )
  {
    QgsRasterLayer *layer = new QgsRasterLayer( outputFile, tr( "Viewshed [%1]" ).arg( center.toString() ) );
    QgsColorRampShader *rampShader = new QgsColorRampShader();
    rampShader->setColorRampType( QgsColorRampShader::Exact );
    if ( displayVisible )
    {
      QList<QgsColorRampShader::ColorRampItem> colorRampItems = QList<QgsColorRampShader::ColorRampItem>()
          << QgsColorRampShader::ColorRampItem( 255, QColor( 0, 255, 0 ), tr( "Visible" ) );
      rampShader->setSourceColorRamp( new QgsPresetSchemeColorRamp( {QColor( 0, 255, 0 )} ) );
      rampShader->setColorRampItemList( colorRampItems );
    }
    else
    {
      QList<QgsColorRampShader::ColorRampItem> colorRampItems = QList<QgsColorRampShader::ColorRampItem>()
          << QgsColorRampShader::ColorRampItem( 0, QColor( 255, 0, 0 ), tr( "Invisible" ) );
      rampShader->setSourceColorRamp( new QgsPresetSchemeColorRamp( {QColor( 255, 0, 0 )} ) );
      rampShader->setColorRampItemList( colorRampItems );
    }
    QgsRasterShader *shader = new QgsRasterShader();
    shader->setRasterShaderFunction( rampShader );
    QgsSingleBandPseudoColorRenderer *renderer = new QgsSingleBandPseudoColorRenderer( 0, 1, shader );
    renderer->setClassificationMin( displayVisible ? 255 : 0 );
    renderer->setClassificationMax( displayVisible ? 255 : 0 );
    layer->setRenderer( renderer );
    QgsProject::instance()->addMapLayer( layer );

    KadasPinItem *pin = new KadasPinItem( canvasCrs );
    pin->associateToLayer( layer );
    pin->setPosition( KadasItemPos::fromPoint( center ) );
    KadasMapCanvasItemManager::addItem( pin );
  }
  else if ( !errMsg.isEmpty() )
  {
    QMessageBox::critical( 0, tr( "Error" ), tr( "Failed to compute viewshed: %1" ).arg( errMsg ) );
  }
  clear();
}

void KadasMapToolViewshed::adjustRadius( double newRadius )
{
  QgsUnitTypes::DistanceUnit measureUnit = QgsUnitTypes::DistanceMeters;
  QgsUnitTypes::DistanceUnit targetUnit = canvas()->mapSettings().destinationCrs().mapUnits();
  newRadius *= QgsUnitTypes::fromUnitToUnitFactor( measureUnit, targetUnit );

  KadasCircularSectorItem *item = dynamic_cast<KadasCircularSectorItem *>( mutableItem() );
  if ( !item )
  {
    return;
  }

  KadasCircularSectorItem::State *state = item->constState()->clone();
  state->radii.last() = newRadius;
  item->setState( state );
  delete state;
}
