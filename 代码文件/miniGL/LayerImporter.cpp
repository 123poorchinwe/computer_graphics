#include "LayerImporter.h"
#include "GeoDefine.h"

#include "..\\3rdParty\\gdal\\include\\ogrsf_frmts.h"
#include "..\\3rdParty\\gdal\\include\\ogr_feature.h"
#include "..\\3rdParty\\gdal\\include\\gdal.h"
#include "..\\3rdParty\\gdal\\include\\ogr_api.h"
#include <assert.h>


#pragma comment(lib,"../3rdParty/gdal/lib/gdal_i.lib")

Layer* LayerImporter::importShpLayer(const char* path , const char* labelFieldName )
{
	GDALAllRegister();//ע��������֪������
	OGRRegisterAll();//��չOGR��ʽ����
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//ʹgdal��֧������·���������ֶ�
	CPLSetConfigOption("SHAPE_ENCODING", "");//��ȷд���������Ե�shp


	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
	OGRDataSource* poDS = poDriver->Open( path );
	if( poDS == NULL ) return NULL;

	OGRLayer* poLayer = poDS->GetLayer(0);
	if( poLayer == NULL ) return NULL;

	OGREnvelope ev;
	poLayer->GetExtent(&ev);//��ȡ��Ҫ�ص�������Χ
	Layer* pLayer = new Layer( (GeomType)poLayer->GetGeomType());
	pLayer->setEnvelop( ev.MinX, ev.MinY, ev. MaxX, ev.MaxY );

	int labelFieldIndex = -1;
	if( strcmp( labelFieldName, "") != 0 )
		labelFieldIndex = poLayer->GetLayerDefn()->GetFieldIndex( labelFieldName );
	//Ҫ�������
	OGRFeature* pFeature;
	//wkt����ͶӰ����ϵ
	while ((pFeature = poLayer->GetNextFeature()) )
	{
		OGRGeometry* pOGRGeometry = pFeature->GetGeometryRef();

		switch (pOGRGeometry->getGeometryType())
		{
		case wkbPoint:
			{
				OGRPoint* pOGRPoint = (OGRPoint*)pOGRGeometry;
				PointGeometry* pGeometry = new PointGeometry( pOGRPoint->getX(),pOGRPoint->getY() ) ;
				if( labelFieldIndex >= 0 )
					pGeometry->label = pFeature->GetFieldAsString( labelFieldIndex );
				pLayer->addGeometry( pGeometry , false);
			}
			break;
		case wkbLineString:
			{
				OGRLineString* pOGRLineString = (OGRLineString*)pOGRGeometry;
				PolylineGeometry* pGeometry = new PolylineGeometry( ) ;
				for (int i = 0, size = pOGRLineString->getNumPoints(); i < size ; ++i)
				{
					pGeometry->addPoint( pOGRLineString->getX(i),pOGRLineString->getY(i) );//��ͼ�㣬���ϻ�ȡ����
				}
				if( labelFieldIndex >= 0 )
					pGeometry->label = pFeature->GetFieldAsString( labelFieldIndex );
				pLayer->addGeometry( pGeometry , false);
			}
			break;
		case wkbPolygon:
			{
				OGRPolygon* pOGRPolygon = (OGRPolygon*)pOGRGeometry;
				PolygonGeometry* pGeometry = new PolygonGeometry( ) ;
				OGRLinearRing* ring = pOGRPolygon->getExteriorRing();
				for (int i = 0, size = ring->getNumPoints(); i < size ; ++i)
				{
					//assert(ring->getX(i) < 120 && ring->getY(i) < 32);
					pGeometry->addPoint( ring->getX(i),ring->getY(i) );
				}		
				if( labelFieldIndex >= 0 )
					pGeometry->label = pFeature->GetFieldAsString( labelFieldIndex );
				pLayer->addGeometry( pGeometry , false);
			}
			break;
		}
		OGRFeature::DestroyFeature(pFeature);

		//static int ii = 0;
		//++ii;
		//if( ii > 0 ) break;
	}

	OGRDataSource::DestroyDataSource(poDS);
	OGRCleanupAll();

	return pLayer;
}
