/***************************************************************************
    kadasguidegridlayer.cpp
    -----------------------
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
#include <QDesktopWidget>
#include <QMenu>

#include <qgis/qgsapplication.h>
#include <qgis/qgslinestring.h>
#include <qgis/qgsmaplayerrenderer.h>
#include <qgis/qgsmapsettings.h>
#include <qgis/qgspolygon.h>
#include <qgis/qgssymbollayerutils.h>

#include <kadas/app/guidegrid/kadasguidegridlayer.h>

static QString gridLabel( QChar firstChar, int offset )
{
  if ( firstChar >= '0' && firstChar <= '9' )
  {
    return QString::number( firstChar.digitValue() + offset );
  }
  else
  {
    QString label;
    offset += firstChar.toLatin1() - 'A' + 1;
    do
    {
      offset -= 1;
      int res = offset % 26;
      label.prepend( QChar( 'A' + res ) );
      offset /= 26;
    }
    while ( offset > 0 );
    return label;
  }
}

class KadasGuideGridLayer::Renderer : public QgsMapLayerRenderer
{
  public:
    Renderer( KadasGuideGridLayer *layer, QgsRenderContext &rendererContext )
      : QgsMapLayerRenderer( layer->id() )
      , mLayer( layer )
      , mRendererContext( rendererContext )
    {}

    bool render() override
    {
      if ( mLayer->mRows == 0 || mLayer->mCols == 0 )
      {
        return true;
      }
      bool previewJob = mRendererContext.flags() & Qgis::RenderContextFlag::RenderPreviewJob;

      mRendererContext.painter()->save();
      mRendererContext.painter()->setOpacity( mLayer->opacity() );
      mRendererContext.painter()->setCompositionMode( QPainter::CompositionMode_Source );
      mRendererContext.painter()->setPen( QPen( mLayer->mColor, mLayer->mLineWidth ) );
      mRendererContext.painter()->setBrush( mLayer->mColor );

      const QVariantMap &flags = mRendererContext.customRenderingFlags();
      bool adaptLabelsToScreen = !( flags["globe"].toBool() || flags["kml"].toBool() );

      QColor bufferColor = ( 0.2126 * mLayer->mColor.red() + 0.7152 * mLayer->mColor.green() + 0.0722 * mLayer->mColor.blue() ) > 128 ? Qt::black : Qt::white;
      double dpiScale = double( mRendererContext.painter()->device()->logicalDpiX() ) / qApp->desktop()->logicalDpiX();

      QFont smallFont;
      smallFont.setPixelSize( 0.5 * mLayer->mFontSize * dpiScale );
      QFontMetrics smallFontMetrics( smallFont );

      QFont font;
      font.setPixelSize( mLayer->mFontSize * dpiScale );
      QFontMetrics fontMetrics( font );

      const int labelBoxSize = fontMetrics.height();
      const int smallLabelBoxSize = smallFontMetrics.height();

      QgsCoordinateTransform crst = mRendererContext.coordinateTransform();
      const QgsMapToPixel &mapToPixel = mRendererContext.mapToPixel();
      const QgsRectangle &gridRect = mLayer->mGridRect;
      QgsPoint pTL = QgsPoint( mRendererContext.mapExtent().xMinimum(), mRendererContext.mapExtent().yMaximum() );
      QgsPoint pBR = QgsPoint( mRendererContext.mapExtent().xMaximum(), mRendererContext.mapExtent().yMinimum() );
      QPointF screenTL = mapToPixel.transform( crst.transform( pTL ) ).toQPointF();
      QPointF screenBR = mapToPixel.transform( crst.transform( pBR ) ).toQPointF();
      QRectF screenRect( screenTL, screenBR );

      double ix = gridRect.width() / mLayer->mCols;
      double iy = gridRect.height() / mLayer->mRows;

      // Draw vertical lines
      QPolygonF vLine1 = vScreenLine( gridRect.xMinimum(), iy );
      {
        QPainterPath path;
        path.addPolygon( vLine1 );
        mRendererContext.painter()->drawPath( path );
      }
      double sy1 = adaptLabelsToScreen ? std::max( vLine1.first().y(), screenRect.top() ) : vLine1.first().y();
      double sy2 = adaptLabelsToScreen ? std::min( vLine1.last().y(), screenRect.bottom() ) : vLine1.last().y();
      QuadrantLabeling quadrantLabeling = mLayer->mQuadrantLabeling;
      for ( int col = 1; col <= mLayer->mCols; ++col )
      {
        double x2 = gridRect.xMinimum() + col * ix;
        QPolygonF vLine2 = vScreenLine( x2, iy );
        QPainterPath path;
        path.addPolygon( vLine2 );
        mRendererContext.painter()->drawPath( path );

        if ( !previewJob )
        {
          double sx1 = vLine1.first().x();
          double sx2 = vLine2.first().x();
          QString label = gridLabel( mLayer->mColChar, col - 1 );
          if ( mLayer->mLabelingPos == LabelsOutside && vLine1.first().y() - labelBoxSize > screenRect.top() )
          {
            drawGridLabel( 0.5 * ( sx1 + sx2 ), sy1 - 0.5 * labelBoxSize, label, font, fontMetrics, bufferColor );
          }
          else if ( sy1 < vLine1.last().y() - 2 * labelBoxSize )
          {
            drawGridLabel( 0.5 * ( sx1 + sx2 ), sy1 + 0.5 * labelBoxSize, label, font, fontMetrics, bufferColor );
          }
          if ( mLayer->mLabelingPos == LabelsOutside && vLine1.last().y() + labelBoxSize < screenRect.bottom() )
          {
            drawGridLabel( 0.5 * ( sx1 + sx2 ), sy2 + 0.5 * labelBoxSize, label, font, fontMetrics, bufferColor );
          }
          else if ( sy2 > vLine1.first().y() + 2 * labelBoxSize )
          {
            drawGridLabel( 0.5 * ( sx1 + sx2 ), sy2 - 0.5 * labelBoxSize, label, font, fontMetrics, bufferColor );
          }
        }

        if ( quadrantLabeling != DontLabelQuadrants )
        {
          mRendererContext.painter()->save();
          mRendererContext.painter()->setPen( QPen( mLayer->mColor, mLayer->mLineWidth, Qt::DashLine ) );
          QSizeF smallLabelBox( smallLabelBoxSize, smallLabelBoxSize );
          QPolygonF vLineMid;
          for ( int i = 0, n = vLine1.size(); i < n; ++i )
          {
            vLineMid.append( 0.5 * ( vLine1.at( i ) + vLine2.at( i ) ) );
            if ( i < n - 1 && 0.4 * qAbs( vLine1.at( i ).x() - vLine2.at( i ).x() ) > smallLabelBoxSize )
            {
              drawGridLabel( vLine1.at( i ).x() + 0.5 * smallLabelBoxSize, vLine1.at( i ).y() + 0.5 * smallLabelBoxSize, "A", smallFont, smallFontMetrics, bufferColor );
              drawGridLabel( vLine2.at( i ).x() - 0.5 * smallLabelBoxSize, vLine2.at( i ).y() + 0.5 * smallLabelBoxSize, "B", smallFont, smallFontMetrics, bufferColor );
              drawGridLabel( vLine1.at( i + 1 ).x() + 0.5 * smallLabelBoxSize, vLine1.at( i + 1 ).y() - 0.5 * smallLabelBoxSize, "D", smallFont, smallFontMetrics, bufferColor );
              drawGridLabel( vLine2.at( i + 1 ).x() - 0.5 * smallLabelBoxSize, vLine2.at( i + 1 ).y() - 0.5 * smallLabelBoxSize, "C", smallFont, smallFontMetrics, bufferColor );
            }
            if ( quadrantLabeling == LabelOneQuadrant )
            {
              vLineMid.append( 0.5 * ( vLine1.at( i + 1 ) + vLine2.at( i + 1 ) ) );
              quadrantLabeling = DontLabelQuadrants;
              break;
            }
          }
          QPainterPath path;
          path.addPolygon( vLineMid );
          mRendererContext.painter()->drawPath( path );
          mRendererContext.painter()->restore();
        }

        vLine1 = vLine2;
      }

      // Draw horizontal lines
      QPolygonF hLine1 = hScreenLine( gridRect.yMaximum(), ix );
      {
        QPainterPath path;
        path.addPolygon( hLine1 );
        mRendererContext.painter()->drawPath( path );
      }
      double sx1 = adaptLabelsToScreen ? std::max( hLine1.first().x(), screenRect.left() ) : hLine1.first().x();
      double sx2 = adaptLabelsToScreen ? std::min( hLine1.last().x(), screenRect.right() ) : hLine1.last().x();
      quadrantLabeling = mLayer->mQuadrantLabeling;
      for ( int row = 1; row <= mLayer->mRows; ++row )
      {
        double y = gridRect.yMaximum() - row * iy;
        QPolygonF hLine2 = hScreenLine( y, ix );
        QPainterPath path;
        path.addPolygon( hLine2 );
        mRendererContext.painter()->drawPath( path );

        if ( !previewJob )
        {
          double sy1 = hLine1.first().y();
          double sy2 = hLine2.first().y();
          QString label = gridLabel( mLayer->mRowChar, row - 1 );
          if ( mLayer->mLabelingPos == LabelsOutside && hLine1.first().x() - labelBoxSize > screenRect.left() )
          {
            drawGridLabel( sx1 - 0.5 * labelBoxSize, 0.5 * ( sy1 + sy2 ), label, font, fontMetrics, bufferColor );
          }
          else if ( sx1 < vLine1.last().x() - 2 * labelBoxSize )
          {
            drawGridLabel( sx1 + 0.5 * labelBoxSize, 0.5 * ( sy1 + sy2 ), label, font, fontMetrics, bufferColor );
          }
          if ( mLayer->mLabelingPos == LabelsOutside && hLine1.last().x() + labelBoxSize < screenRect.right() )
          {
            drawGridLabel( sx2 + 0.5 * labelBoxSize, 0.5 * ( sy1 + sy2 ), label, font, fontMetrics, bufferColor );
          }
          else if ( sx2 > hLine1.first().x() + 2 * labelBoxSize )
          {
            drawGridLabel( sx2 - 0.5 * labelBoxSize, 0.5 * ( sy1 + sy2 ), label, font, fontMetrics, bufferColor );
          }
        }

        if ( quadrantLabeling != DontLabelQuadrants )
        {
          mRendererContext.painter()->save();
          mRendererContext.painter()->setPen( QPen( mLayer->mColor, mLayer->mLineWidth, Qt::DashLine ) );
          QPolygonF hLineMid;
          if ( quadrantLabeling == LabelOneQuadrant )
          {
            hLineMid.append( 0.5 * ( hLine1.at( 0 ) + hLine2.at( 0 ) ) );
            hLineMid.append( 0.5 * ( hLine1.at( 1 ) + hLine2.at( 1 ) ) );
            quadrantLabeling = DontLabelQuadrants;
          }
          else
          {
            for ( int i = 0, n = hLine1.size(); i < n; ++i )
            {
              hLineMid.append( 0.5 * ( hLine1.at( i ) + hLine2.at( i ) ) );
            }
          }
          QPainterPath path;
          path.addPolygon( hLineMid );
          mRendererContext.painter()->drawPath( path );
          mRendererContext.painter()->restore();
        }

        hLine1 = hLine2;
      }
      mRendererContext.painter()->restore();
      return true;
    }
    void drawGridLabel( double x, double y, const QString &text, const QFont &font, const QFontMetrics &metrics, const QColor &bufferColor )
    {
      QPainterPath path;
      x -= 0.5 * metrics.horizontalAdvance( text );
      y =  y - metrics.descent() + 0.5 * metrics.height();
      path.addText( x, y, font, text );
      mRendererContext.painter()->save();
      mRendererContext.painter()->setPen( QPen( bufferColor, qRound( mLayer->mFontSize / 8. ) ) );
      mRendererContext.painter()->drawPath( path );
      mRendererContext.painter()->setPen( Qt::NoPen );
      mRendererContext.painter()->drawPath( path );
      mRendererContext.painter()->restore();
    }

  private:
    KadasGuideGridLayer *mLayer;
    QgsRenderContext &mRendererContext;

    QPolygonF vScreenLine( double x, double iy ) const
    {
      QgsCoordinateTransform crst = mRendererContext.coordinateTransform();
      const QgsMapToPixel &mapToPixel = mRendererContext.mapToPixel();
      const QgsRectangle &gridRect = mLayer->mGridRect;
      QPolygonF screenPoints;
      for ( int row = 0; row <= mLayer->mRows; ++row )
      {
        QgsPoint p( x, gridRect.yMaximum() - row * iy );
        QPointF screenPoint = mapToPixel.transform( crst.transform( p ) ).toQPointF();
        screenPoints.append( screenPoint );
      }
      return screenPoints;
    }
    QPolygonF hScreenLine( double y, double ix ) const
    {
      QgsCoordinateTransform crst = mRendererContext.coordinateTransform();
      const QgsMapToPixel &mapToPixel = mRendererContext.mapToPixel();
      const QgsRectangle &gridRect = mLayer->mGridRect;
      QPolygonF screenPoints;
      for ( int col = 0; col <= mLayer->mCols; ++col )
      {
        QgsPoint p( gridRect.xMinimum() + col * ix, y );
        QPointF screenPoint = mapToPixel.transform( crst.transform( p ) ).toQPointF();
        screenPoints.append( screenPoint );
      }
      return screenPoints;
    }
};

KadasGuideGridLayer::KadasGuideGridLayer( const QString &name )
  : KadasPluginLayer( layerType(), name )
{
  mValid = true;
}

void KadasGuideGridLayer::setup( const QgsRectangle &gridRect, int cols, int rows, const QgsCoordinateReferenceSystem &crs, bool colSizeLocked, bool rowSizeLocked )
{
  mGridRect = gridRect;
  mCols = cols;
  mRows = rows;
  mColSizeLocked = colSizeLocked;
  mRowSizeLocked = rowSizeLocked;
  setCrs( crs, false );
}

KadasGuideGridLayer *KadasGuideGridLayer::clone() const
{
  KadasGuideGridLayer *layer = new KadasGuideGridLayer( name() );
  layer->mTransformContext = mTransformContext;
  layer->mOpacity = mOpacity;
  layer->mGridRect = mGridRect;
  layer->mCols = mCols;
  layer->mRows = mRows;
  layer->mColSizeLocked = mColSizeLocked;
  layer->mRowSizeLocked = mRowSizeLocked;
  layer->mFontSize = mFontSize;
  layer->mColor = mColor;
  layer->mLineWidth = mLineWidth;
  layer->mRowChar = mRowChar;
  layer->mColChar = mColChar;
  layer->mLabelingPos = mLabelingPos;
  layer->mQuadrantLabeling = mQuadrantLabeling;
  return layer;
}

QgsMapLayerRenderer *KadasGuideGridLayer::createMapRenderer( QgsRenderContext &rendererContext )
{
  return new Renderer( this, rendererContext );
}

bool KadasGuideGridLayer::readXml( const QDomNode &layer_node, QgsReadWriteContext &context )
{
  QDomElement layerEl = layer_node.toElement();
  mLayerName = layerEl.attribute( "title" );
  mOpacity = ( 100. - layerEl.attribute( "transparency" ).toInt() ) / 100.;
  mGridRect.setXMinimum( layerEl.attribute( "xmin" ).toDouble() );
  mGridRect.setYMinimum( layerEl.attribute( "ymin" ).toDouble() );
  mGridRect.setXMaximum( layerEl.attribute( "xmax" ).toDouble() );
  mGridRect.setYMaximum( layerEl.attribute( "ymax" ).toDouble() );
  mCols = layerEl.attribute( "cols" ).toInt();
  mRows = layerEl.attribute( "rows" ).toInt();
  mColSizeLocked = layerEl.attribute( "colSizeLocked", "0" ).toInt();
  mRowSizeLocked = layerEl.attribute( "rowSizeLocked", "0" ).toInt();
  mFontSize = layerEl.attribute( "fontSize" ).toInt();
  mColor = QgsSymbolLayerUtils::decodeColor( layerEl.attribute( "color" ) );
  mLineWidth = layerEl.attribute( "lineWidth", "1" ).toInt();
  mRowChar = layerEl.attribute( "rowChar" ).size() > 0 ? layerEl.attribute( "rowChar" ).at( 0 ) : 'A';
  mColChar = layerEl.attribute( "colChar" ).size() > 0 ? layerEl.attribute( "colChar" ).at( 0 ) : '1';
  mLabelingPos = static_cast<LabelingPos>( layerEl.attribute( "labelingPos" ).toInt() );
  if ( layerEl.hasAttribute( "quadrantLabeling" ) )
  {
    mQuadrantLabeling = static_cast<QuadrantLabeling>( layerEl.attribute( "quadrantLabeling" ).toInt() );
  }
  else
  {
    mQuadrantLabeling = layerEl.attribute( "labelQuadrans" ).toInt() == 1 ? LabelAllQuadrants : DontLabelQuadrants;
  }
  if ( !layerEl.attribute( "labellingMode" ).isEmpty() )
  {
    // Compatibility
    enum LabellingMode { LABEL_A_1, LABEL_1_A };
    int labellingMode =  static_cast<LabellingMode>( layerEl.attribute( "labellingMode" ).toInt() );
    if ( labellingMode == LABEL_A_1 )
    {
      mRowChar = 'A';
      mColChar = '1';
    }
    else
    {
      mRowChar = '1';
      mColChar = 'A';
    }
  }

  setCrs( QgsCoordinateReferenceSystem( layerEl.attribute( "crs" ) ) );
  return true;
}

QList<KadasGuideGridLayer::IdentifyResult> KadasGuideGridLayer::identify( const QgsPointXY &mapPos, const QgsMapSettings &mapSettings )
{
  QgsCoordinateTransform crst( mapSettings.destinationCrs(), crs(), mTransformContext );
  QgsPointXY pos = crst.transform( mapPos );

  double colWidth = ( mGridRect.xMaximum() - mGridRect.xMinimum() ) / mCols;
  double rowHeight = ( mGridRect.yMaximum() - mGridRect.yMinimum() ) / mRows;

  int i = std::floor( ( pos.x() - mGridRect.xMinimum() ) / colWidth );
  int j = std::floor( ( mGridRect.yMaximum() - pos.y() ) / rowHeight );

  QgsPolygon *bbox = new QgsPolygon();
  QgsLineString *ring = new QgsLineString();
  ring->setPoints(
    QgsPointSequence()
    << QgsPoint( mGridRect.xMinimum() + i * colWidth,     mGridRect.yMaximum() - j * rowHeight )
    << QgsPoint( mGridRect.xMinimum() + ( i + 1 ) * colWidth, mGridRect.yMaximum() - j * rowHeight )
    << QgsPoint( mGridRect.xMinimum() + ( i + 1 ) * colWidth, mGridRect.yMaximum() - ( j + 1 ) * rowHeight )
    << QgsPoint( mGridRect.xMinimum() + i * colWidth,     mGridRect.yMaximum() - ( j + 1 ) * rowHeight )
    << QgsPoint( mGridRect.xMinimum() + i * colWidth,     mGridRect.yMaximum() - j * rowHeight )
  );
  bbox->setExteriorRing( ring );
  QMap<QString, QVariant> attrs;

  QString text = tr( "Cell %1, %2" ).arg( gridLabel( mRowChar, j ) ).arg( gridLabel( mColChar, i ) );
  if ( mQuadrantLabeling != DontLabelQuadrants )
  {
    bool left = pos.x() <= mGridRect.xMinimum() + ( i + 0.5 ) * colWidth;
    bool top = pos.y() >= mGridRect.yMaximum() - ( j + 0.5 ) * rowHeight;
    QString quadrantLabel;
    if ( left )
    {
      quadrantLabel = top ? "A" : "D";
    }
    else
    {
      quadrantLabel = top ? "B" : "C";
    }
    text += tr( " (Quadrant %1)" ).arg( quadrantLabel );
  }

  return QList<IdentifyResult>() << IdentifyResult( text, attrs, QgsGeometry( bbox ) );
}

bool KadasGuideGridLayer::writeXml( QDomNode &layer_node, QDomDocument & /*document*/, const QgsReadWriteContext &context ) const
{
  QDomElement layerEl = layer_node.toElement();
  layerEl.setAttribute( "type", "plugin" );
  layerEl.setAttribute( "name", layerTypeKey() );
  layerEl.setAttribute( "title", name() );
  layerEl.setAttribute( "transparency", 100. - mOpacity * 100. );
  layerEl.setAttribute( "xmin", mGridRect.xMinimum() );
  layerEl.setAttribute( "ymin", mGridRect.yMinimum() );
  layerEl.setAttribute( "xmax", mGridRect.xMaximum() );
  layerEl.setAttribute( "ymax", mGridRect.yMaximum() );
  layerEl.setAttribute( "cols", mCols );
  layerEl.setAttribute( "rows", mRows );
  layerEl.setAttribute( "colSizeLocked", mColSizeLocked ? 1 : 0 );
  layerEl.setAttribute( "rowSizeLocked", mRowSizeLocked ? 1 : 0 );
  layerEl.setAttribute( "crs", crs().authid() );
  layerEl.setAttribute( "fontSize", mFontSize );
  layerEl.setAttribute( "color", QgsSymbolLayerUtils::encodeColor( mColor ) );
  layerEl.setAttribute( "lineWidth", mLineWidth );
  layerEl.setAttribute( "colChar", QString( mColChar ) );
  layerEl.setAttribute( "rowChar", QString( mRowChar ) );
  layerEl.setAttribute( "labelingPos", mLabelingPos );
  layerEl.setAttribute( "quadrantLabeling", mQuadrantLabeling );
  return true;
}

void KadasGuideGridLayerType::addLayerTreeMenuActions( QMenu *menu, QgsPluginLayer *layer ) const
{
  menu->addAction( QgsApplication::getThemeIcon( "/mActionToggleEditing.svg" ), tr( "Edit" ), this, [this, layer]
  {
    mActionGuideGridTool->trigger();
  } );
}
