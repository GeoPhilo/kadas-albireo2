/***************************************************************************
    kadasbullseyelayer.cpp
    ----------------------
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

#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/GeodesicLine.hpp>

#include <qgis/qgsapplication.h>
#include <qgis/qgscoordinatereferencesystem.h>
#include <qgis/qgslayertreeview.h>
#include <qgis/qgsmapcanvas.h>
#include <qgis/qgsmaplayerrenderer.h>
#include <qgis/qgssymbollayerutils.h>

#include <kadas/app/bullseye/kadasbullseyelayer.h>
#include <kadas/app/bullseye/kadasmaptoolbullseye.h>


class KadasBullseyeLayer::Renderer : public QgsMapLayerRenderer
{
  public:
    Renderer( KadasBullseyeLayer *layer, QgsRenderContext &rendererContext )
      : QgsMapLayerRenderer( layer->id() )
      , mLayer( layer )
      , mRendererContext( rendererContext )
      , mGeod( GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f() )
    {
      mDa.setEllipsoid( "WGS84" );
      mDa.setSourceCrs( QgsCoordinateReferenceSystem( "EPSG:4326" ), mRendererContext.transformContext() );
    }

    bool render() override
    {
      if ( mLayer->mRings <= 0 || mLayer->mInterval <= 0 )
      {
        return true;
      }

      const QgsMapToPixel &mapToPixel = mRendererContext.mapToPixel();
      double dpiScale = double( mRendererContext.painter()->device()->logicalDpiX() ) / qApp->desktop()->logicalDpiX();

      mRendererContext.painter()->save();
      mRendererContext.painter()->setOpacity( mLayer->mOpacity );
      mRendererContext.painter()->setCompositionMode( QPainter::CompositionMode_Source );
      mRendererContext.painter()->setPen( QPen( mLayer->mColor, mLayer->mLineWidth ) );
      QFont font = mRendererContext.painter()->font();
      font.setPixelSize( mLayer->mFontSize * dpiScale );
      QFontMetrics metrics( mRendererContext.painter()->font() );
      QColor bufferColor = ( 0.2126 * mLayer->mColor.red() + 0.7152 * mLayer->mColor.green() + 0.0722 * mLayer->mColor.blue() ) > 128 ? Qt::black : Qt::white;

      QgsCoordinateReferenceSystem crsWgs84( "EPSG:4326" );
      QgsCoordinateTransform ct( mLayer->crs(), crsWgs84, mRendererContext.transformContext() );
      QgsCoordinateTransform rct( crsWgs84, mRendererContext.coordinateTransform().destinationCrs(), mRendererContext.transformContext() );

      // Draw rings
      QgsPointXY wgsCenter = ct.transform( mLayer->mCenter );
      double intervalUnit2meters = QgsUnitTypes::fromUnitToUnitFactor( mLayer->mIntervalUnit, QgsUnitTypes::DistanceMeters );
      for ( int iRing = 0; iRing < mLayer->mRings; ++iRing )
      {
        double radMeters = mLayer->mInterval * ( 1 + iRing ) * intervalUnit2meters;

        QPolygonF poly;
        for ( int a = 0; a <= 360; ++a )
        {
          QgsPointXY wgsPoint = mDa.computeSpheroidProject( wgsCenter, radMeters, a / 180. * M_PI );
          QgsPointXY mapPoint = rct.transform( wgsPoint );
          poly.append( mapToPixel.transform( mapPoint ).toQPointF() );
        }
        QPainterPath path;
        path.addPolygon( poly );
        mRendererContext.painter()->drawPath( path );
        if ( mLayer->mLabelRings )
        {
          QString label = QString( "%1 %2" ).arg( ( iRing + 1 ) * mLayer->mInterval, 0, 'f', 2 ).arg( QgsUnitTypes::toAbbreviatedString( mLayer->mIntervalUnit ) );
          double x = poly.last().x() - 0.5 * metrics.horizontalAdvance( label );
          drawGridLabel( x, poly.last().y() - 0.25 * metrics.height(), label, font, bufferColor );
        }
      }

      // Draw axes
      double axisRadiusMeters = mLayer->mInterval * ( mLayer->mRings + 1 ) * intervalUnit2meters;
      for ( int bearing = 0; bearing < 360; bearing += mLayer->mAxesInterval )
      {
        QgsPointXY wgsPoint = mDa.computeSpheroidProject( wgsCenter, axisRadiusMeters, bearing / 180. * M_PI );
        GeographicLib::GeodesicLine line = mGeod.InverseLine( wgsCenter.y(), wgsCenter.x(), wgsPoint.y(), wgsPoint.x() );
        double dist = line.Distance();
        double sdist = 100000; // ~100km segments
        int nSegments = std::max( 1, int( std::ceil( dist / sdist ) ) );
        QPolygonF poly;
        for ( int iSeg = 0; iSeg < nSegments; ++iSeg )
        {
          double lat, lon;
          line.Position( iSeg * sdist, lat, lon );
          QgsPointXY mapPoint = rct.transform( QgsPoint( lon, lat ) );
          poly.append( mapToPixel.transform( mapPoint ).toQPointF() );
        }
        double lat, lon;
        line.Position( dist, lat, lon );
        QgsPointXY mapPoint = rct.transform( QgsPoint( lon, lat ) );
        poly.append( mapToPixel.transform( mapPoint ).toQPointF() );
        QPainterPath path;
        path.addPolygon( poly );
        mRendererContext.painter()->drawPath( path );
        if ( mLayer->mLabelAxes )
        {
          QString label = QString( "%1°" ).arg( bearing );
          int n = poly.size();
          double dx = n > 1 ? poly[n - 1].x() - poly[n - 2].x() : 0;
          double dy = n > 1 ? poly[n - 1].y() - poly[n - 2].y() : 0;
          double l = std::sqrt( dx * dx + dy * dy );
          double d = mLayer->mFontSize;
          double w = metrics.horizontalAdvance( label );
          double x = n < 2 ? poly.last().x() : poly.last().x() + d * dx / l;
          double y = n < 2 ? poly.last().y() : poly.last().y() + d * dy / l;
          drawGridLabel( x - w, y + 0.5 * metrics.ascent(), label, font, bufferColor );
        }
      }
      if ( mLayer->mLabelQuadrants )
      {
        const char firstLetter = 'F';
        QList<char> labelChars = {firstLetter};
        for ( int iRing = 0; iRing < mLayer->mRings; ++iRing )
        {
          double r = mLayer->mInterval * ( 0.5 + iRing ) * intervalUnit2meters;
          for ( int bearing = 0; bearing < 360; bearing += mLayer->mAxesInterval )
          {
            double a = bearing + 0.5 * mLayer->mAxesInterval;
            QgsPointXY wgsPoint = mDa.computeSpheroidProject( wgsCenter, r, a / 180. * M_PI );
            QgsPointXY mapPoint = rct.transform( wgsPoint );
            QPointF screenPoint = mapToPixel.transform( mapPoint ).toQPointF();
            QString label;
            for ( char c : labelChars )
            {
              label += c;
            }
            drawGridLabel( screenPoint.x(), screenPoint.y(), label, font, bufferColor );
            if ( labelChars.last() == 'Z' )
            {
              labelChars.last() = firstLetter;
              labelChars.append( firstLetter );
            }
            else
            {
              ++labelChars.last();
              if ( labelChars.last() == 'I' || labelChars.last() == 'O' )
              {
                ++labelChars.last();
              }
            }
          }
        }
      }

      mRendererContext.painter()->restore();
      return true;
    }

  private:
    KadasBullseyeLayer *mLayer;
    QgsRenderContext &mRendererContext;
    QgsDistanceArea mDa;
    GeographicLib::Geodesic mGeod;

    QPair<QPointF, QPointF> screenLine( const QgsPoint &p1, const QgsPoint &p2 ) const
    {
      const QgsMapToPixel &mapToPixel = mRendererContext.mapToPixel();
      QPointF sp1 = mapToPixel.transform( mRendererContext.coordinateTransform().transform( p1 ) ).toQPointF();
      QPointF sp2 = mapToPixel.transform( mRendererContext.coordinateTransform().transform( p2 ) ).toQPointF();
      return qMakePair( sp1, sp2 );
    }
    void drawGridLabel( double x, double y, const QString &text, const QFont &font, const QColor &bufferColor )
    {
      QPainterPath path;
      path.addText( x, y, font, text );
      mRendererContext.painter()->save();
      mRendererContext.painter()->setBrush( mLayer->mColor );
      mRendererContext.painter()->setPen( QPen( bufferColor, qRound( mLayer->mFontSize / 8. ) ) );
      mRendererContext.painter()->drawPath( path );
      mRendererContext.painter()->setPen( Qt::NoPen );
      mRendererContext.painter()->drawPath( path );
      mRendererContext.painter()->restore();
    }
};

KadasBullseyeLayer::KadasBullseyeLayer( const QString &name )
  : KadasPluginLayer( layerType(), name )
{
  mValid = true;
}

void KadasBullseyeLayer::setup( const QgsPointXY &center, const QgsCoordinateReferenceSystem &crs, int rings, double interval, QgsUnitTypes::DistanceUnit intervalUnit, double axesInterval )
{
  mCenter = center;
  mRings = rings;
  mInterval = interval;
  mIntervalUnit = intervalUnit;
  mAxesInterval = axesInterval;
  setCrs( crs, false );
}

KadasBullseyeLayer *KadasBullseyeLayer::clone() const
{
  KadasBullseyeLayer *layer = new KadasBullseyeLayer( name() );
  layer->mTransformContext = mTransformContext;
  layer->mOpacity = mOpacity;
  layer->mCenter = mCenter;
  layer->mRings = mRings;
  layer->mInterval = mInterval;
  layer->mIntervalUnit = mIntervalUnit;
  layer->mAxesInterval = mAxesInterval;
  layer->mColor = mColor;
  layer->mFontSize = mFontSize;
  layer->mLabelAxes = mLabelAxes;
  layer->mLabelQuadrants = mLabelQuadrants;
  layer->mLabelRings = mLabelRings;
  layer->mLineWidth = mLineWidth;
  return layer;
}

QgsMapLayerRenderer *KadasBullseyeLayer::createMapRenderer( QgsRenderContext &rendererContext )
{
  return new Renderer( this, rendererContext );
}

QgsRectangle KadasBullseyeLayer::extent() const
{
  QgsDistanceArea da;
  double radius = mRings * mInterval * QgsUnitTypes::fromUnitToUnitFactor( mIntervalUnit, QgsUnitTypes::DistanceMeters );
  radius *= QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceMeters, crs().mapUnits() );
  return QgsRectangle( mCenter.x() - radius, mCenter.y() - radius, mCenter.x() + radius, mCenter.y() + radius );
}

bool KadasBullseyeLayer::readXml( const QDomNode &layer_node, QgsReadWriteContext &context )
{
  QDomElement layerEl = layer_node.toElement();
  mLayerName = layerEl.attribute( "title" );
  mOpacity = ( 100. - layerEl.attribute( "transparency" ).toInt() ) / 100.;
  mCenter.setX( layerEl.attribute( "x" ).toDouble() );
  mCenter.setY( layerEl.attribute( "y" ).toDouble() );
  mRings = layerEl.attribute( "rings" ).toInt();
  mAxesInterval = layerEl.attribute( "axes" ).toDouble();
  mInterval = layerEl.attribute( "interval" ).toDouble();
  if ( layerEl.hasAttribute( "intervalUnit" ) )
  {
    mIntervalUnit = QgsUnitTypes::decodeDistanceUnit( layerEl.attribute( "intervalUnit" ) );
  }
  else
  {
    mIntervalUnit = QgsUnitTypes::DistanceNauticalMiles;
  }
  mFontSize = layerEl.attribute( "fontSize" ).toInt();
  mLineWidth = layerEl.attribute( "lineWidth" ).toInt();
  mColor = QgsSymbolLayerUtils::decodeColor( layerEl.attribute( "color" ) );
  if ( layerEl.hasAttribute( "labellingMode" ) )
  {
    // Backwards compatibility with KADAS-2.0
    enum LabellingMode { NO_LABELS, LABEL_AXES, LABEL_RINGS, LABEL_AXES_RINGS };
    LabellingMode labellingMode = static_cast<LabellingMode>( layerEl.attribute( "labellingMode" ).toInt() );
    mLabelAxes = labellingMode == LABEL_AXES || labellingMode == LABEL_AXES_RINGS;
    mLabelQuadrants = false;
    mLabelRings = labellingMode == LABEL_RINGS || labellingMode == LABEL_AXES_RINGS;
  }
  else
  {
    mLabelAxes = layerEl.attribute( "labelAxes" ) == "1";
    mLabelQuadrants = layerEl.attribute( "labelQuadrants" ) == "1";
    mLabelRings = layerEl.attribute( "labelRings" ) == "1";
  }

  setCrs( QgsCoordinateReferenceSystem( layerEl.attribute( "crs" ) ) );
  return true;
}

bool KadasBullseyeLayer::writeXml( QDomNode &layer_node, QDomDocument &document, const QgsReadWriteContext &context ) const
{
  QDomElement layerEl = layer_node.toElement();
  layerEl.setAttribute( "type", "plugin" );
  layerEl.setAttribute( "name", layerTypeKey() );
  layerEl.setAttribute( "title", name() );
  layerEl.setAttribute( "transparency", 100. - 100. * mOpacity );
  layerEl.setAttribute( "x", mCenter.x() );
  layerEl.setAttribute( "y", mCenter.y() );
  layerEl.setAttribute( "rings", mRings );
  layerEl.setAttribute( "axes", mAxesInterval );
  layerEl.setAttribute( "interval", mInterval );
  layerEl.setAttribute( "intervalUnit", QgsUnitTypes::encodeUnit( mIntervalUnit ) );
  layerEl.setAttribute( "crs", crs().authid() );
  layerEl.setAttribute( "fontSize", mFontSize );
  layerEl.setAttribute( "lineWidth", mLineWidth );
  layerEl.setAttribute( "color", QgsSymbolLayerUtils::encodeColor( mColor ) );
  layerEl.setAttribute( "labelAxes", mLabelAxes ? "1" : "0" );
  layerEl.setAttribute( "labelQuadrants", mLabelQuadrants ? "1" : "0" );
  layerEl.setAttribute( "labelRings", mLabelRings ? "1" : "0" );
  return true;
}

void KadasBullseyeLayerType::addLayerTreeMenuActions( QMenu *menu, QgsPluginLayer *layer ) const
{
  menu->addAction( QgsApplication::getThemeIcon( "/mActionToggleEditing.svg" ), tr( "Edit" ), this, [this, layer]
  {
    mActionBullseyeTool->trigger();
  } );
}
