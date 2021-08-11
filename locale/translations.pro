include(translations.pri)

SOURCES += \
../kadas/core/kadaslatlontoutm.h \
../kadas/core/kadascoordinateformat.h \
../kadas/core/kadasalgorithms.h \
../kadas/core/kadasfileserver.h \
../kadas/core/kadasfileserver.cpp \
../kadas/core/kadaslatlontoutm.cpp \
../kadas/core/kadasstatehistory.cpp \
../kadas/core/kadasalgorithms.cpp \
../kadas/core/kadas.cpp \
../kadas/core/kadascoordinateformat.cpp \
../kadas/core/kadas.h \
../kadas/core/kadaspluginlayer.h \
../kadas/core/kadasstatehistory.h \
../kadas/analysis/kadasninecellfilter.cpp \
../kadas/analysis/kadasninecellfilter.h \
../kadas/analysis/kadasviewshedfilter.h \
../kadas/analysis/kadashillshadefilter.cpp \
../kadas/analysis/kadasslopefilter.cpp \
../kadas/analysis/kadashillshadefilter.h \
../kadas/analysis/kadasviewshedfilter.cpp \
../kadas/analysis/kadasslopefilter.h \
../kadas/gui/kadasitemcontextmenuactions.h \
../kadas/gui/kadasbookmarksmenu.h \
../kadas/gui/kadasfloatinginputwidget.cpp \
../kadas/gui/kadasitemlayer.cpp \
../kadas/gui/kadasmapcanvasitem.h \
../kadas/gui/kadasbottombar.cpp \
../kadas/gui/kadasmapwidget.cpp \
../kadas/gui/kadasfeaturepicker.cpp \
../kadas/gui/kadaslayerselectionwidget.cpp \
../kadas/gui/kadassearchbox.h \
../kadas/gui/kadasmapwidget.h \
../kadas/gui/kadasmapcanvasitem.cpp \
../kadas/gui/kadasplugininterface.h \
../kadas/gui/kadasrichtexteditor.h \
../kadas/gui/kadaslayerpropertiesdialog.cpp \
../kadas/gui/kadastextbrowser.cpp \
../kadas/gui/kadasmapcanvasitemmanager.h \
../kadas/gui/kadasmapitemtooltip.h \
../kadas/gui/kadasheightprofiledialog.h \
../kadas/gui/mapitems/kadastextitem.cpp \
../kadas/gui/mapitems/kadaspolygonitem.cpp \
../kadas/gui/mapitems/kadascircleitem.cpp \
../kadas/gui/mapitems/kadaspolygonitem.h \
../kadas/gui/mapitems/kadasrectangleitem.cpp \
../kadas/gui/mapitems/kadasmapitem.h \
../kadas/gui/mapitems/kadasgeometryitem.h \
../kadas/gui/mapitems/kadasselectionrectitem.h \
../kadas/gui/mapitems/kadaspointitem.cpp \
../kadas/gui/mapitems/kadastextitem.h \
../kadas/gui/mapitems/kadasanchoreditem.h \
../kadas/gui/mapitems/kadaspointitem.h \
../kadas/gui/mapitems/kadasrectangleitem.h \
../kadas/gui/mapitems/kadasgeometryitem.cpp \
../kadas/gui/mapitems/kadascoordinatecrossitem.cpp \
../kadas/gui/mapitems/kadasgpxwaypointitem.cpp \
../kadas/gui/mapitems/kadascircularsectoritem.h \
../kadas/gui/mapitems/kadasselectionrectitem.cpp \
../kadas/gui/mapitems/kadascoordinatecrossitem.h \
../kadas/gui/mapitems/kadaslineitem.cpp \
../kadas/gui/mapitems/kadasmapitem.cpp \
../kadas/gui/mapitems/kadascircularsectoritem.cpp \
../kadas/gui/mapitems/kadaslineitem.h \
../kadas/gui/mapitems/kadascircleitem.h \
../kadas/gui/mapitems/kadasgpxwaypointitem.h \
../kadas/gui/mapitems/kadassymbolitem.cpp \
../kadas/gui/mapitems/kadasanchoreditem.cpp \
../kadas/gui/mapitems/kadassymbolitem.h \
../kadas/gui/mapitems/kadaspictureitem.h \
../kadas/gui/mapitems/kadaspictureitem.cpp \
../kadas/gui/mapitems/kadasgpxrouteitem.cpp \
../kadas/gui/mapitems/kadasgpxrouteitem.h \
../kadas/gui/kadaslayerselectionwidget.h \
../kadas/gui/kadascrsselection.cpp \
../kadas/gui/kadascoordinatedisplayer.cpp \
../kadas/gui/kadascrsselection.h \
../kadas/gui/kadasfloatinginputwidget.h \
../kadas/gui/mapitemeditors/kadasgpxwaypointeditor.cpp \
../kadas/gui/mapitemeditors/kadasredliningtexteditor.cpp \
../kadas/gui/mapitemeditors/kadasmapitemeditor.h \
../kadas/gui/mapitemeditors/kadasgpxrouteeditor.h \
../kadas/gui/mapitemeditors/kadasgpxrouteeditor.cpp \
../kadas/gui/mapitemeditors/kadasredliningitemeditor.h \
../kadas/gui/mapitemeditors/kadasmapitemeditor.cpp \
../kadas/gui/mapitemeditors/kadasgpxwaypointeditor.h \
../kadas/gui/mapitemeditors/kadasredliningtexteditor.h \
../kadas/gui/mapitemeditors/kadassymbolattributeseditor.cpp \
../kadas/gui/mapitemeditors/kadasredliningitemeditor.cpp \
../kadas/gui/mapitemeditors/kadassymbolattributeseditor.h \
../kadas/gui/kadascoordinateinput.h \
../kadas/gui/kadasmapitemtooltip.cpp \
../kadas/gui/maptools/kadasmaptoolpan.h \
../kadas/gui/maptools/kadasmaptoolcreateitem.cpp \
../kadas/gui/maptools/kadasmaptoolpan.cpp \
../kadas/gui/maptools/kadasmaptoolheightprofile.cpp \
../kadas/gui/maptools/kadasmaptoolviewshed.cpp \
../kadas/gui/maptools/kadasmaptooledititem.h \
../kadas/gui/maptools/kadasmaptoolheightprofile.h \
../kadas/gui/maptools/kadasmaptooldeleteitems.h \
../kadas/gui/maptools/kadasmaptoolmeasure.cpp \
../kadas/gui/maptools/kadasmaptoolhillshade.cpp \
../kadas/gui/maptools/kadasmaptoolslope.h \
../kadas/gui/maptools/kadasmaptoolselectrect.h \
../kadas/gui/maptools/kadasmaptoolhillshade.h \
../kadas/gui/maptools/kadasmaptoolviewshed.h \
../kadas/gui/maptools/kadasmaptooldeleteitems.cpp \
../kadas/gui/maptools/kadasmaptoolselectrect.cpp \
../kadas/gui/maptools/kadasmaptoolcreateitem.h \
../kadas/gui/maptools/kadasmaptoolmeasure.h \
../kadas/gui/maptools/kadasmaptoolslope.cpp \
../kadas/gui/maptools/kadasmaptooledititemgroup.cpp \
../kadas/gui/maptools/kadasmaptooledititem.cpp \
../kadas/gui/maptools/kadasmaptooledititemgroup.h \
../kadas/gui/catalog/kadasgeoadminrestcatalogprovider.cpp \
../kadas/gui/catalog/kadasvbscatalogprovider.h \
../kadas/gui/catalog/kadasarcgisrestcatalogprovider.h \
../kadas/gui/catalog/kadasarcgisportalcatalogprovider.h \
../kadas/gui/catalog/kadasarcgisrestcatalogprovider.cpp \
../kadas/gui/catalog/kadasarcgisportalcatalogprovider.cpp \
../kadas/gui/catalog/kadasgeoadminrestcatalogprovider.h \
../kadas/gui/catalog/kadasvbscatalogprovider.cpp \
../kadas/gui/ui/kadasgpxrouteeditor.ui \
../kadas/gui/ui/kadasprojecttemplateselectiondialog.ui \
../kadas/gui/ui/kadasgpxwaypointeditor.ui \
../kadas/gui/ui/kadassymbolattributeseditor.ui \
../kadas/gui/ui/kadasredliningtexteditor.ui \
../kadas/gui/ui/kadasredliningitemeditor.ui \
../kadas/gui/ui/kadaslayerpropertiesdialog.ui \
../kadas/gui/kadascatalogprovider.h \
../kadas/gui/kadasbottombar.h \
../kadas/gui/kadasfeaturepicker.h \
../kadas/gui/kadascoordinatedisplayer.h \
../kadas/gui/search/kadaspinsearchprovider.cpp \
../kadas/gui/search/kadaslocationsearchprovider.cpp \
../kadas/gui/search/kadascoordinatesearchprovider.cpp \
../kadas/gui/search/kadasworldlocationsearchprovider.cpp \
../kadas/gui/search/kadaslocationsearchprovider.h \
../kadas/gui/search/kadaspinsearchprovider.h \
../kadas/gui/search/kadasworldlocationsearchprovider.h \
../kadas/gui/search/kadaslocaldatasearchprovider.cpp \
../kadas/gui/search/kadasremotedatasearchprovider.h \
../kadas/gui/search/kadaslocaldatasearchprovider.h \
../kadas/gui/search/kadasremotedatasearchprovider.cpp \
../kadas/gui/search/kadascoordinatesearchprovider.h \
../kadas/gui/kadasbookmarksmenu.cpp \
../kadas/gui/kadaslayerpropertiesdialog.h \
../kadas/gui/kadascatalogbrowser.h \
../kadas/gui/kadasheightprofiledialog.cpp \
../kadas/gui/kadasmapcanvasitemmanager.cpp \
../kadas/gui/kadascatalogprovider.cpp \
../kadas/gui/kadassearchprovider.h \
../kadas/gui/kadasitemlayer.h \
../kadas/gui/milx/kadasmilxeditor.cpp \
../kadas/gui/milx/kadasmilxlayerpropertiespage.cpp \
../kadas/gui/milx/kadasmilxitem.cpp \
../kadas/gui/milx/kadasmilxeditor.h \
../kadas/gui/milx/kadasmilxlayer.cpp \
../kadas/gui/milx/kadasmilxlibrary.h \
../kadas/gui/milx/kadasmilxlayerpropertiespage.h \
../kadas/gui/milx/kadasmilxlayer.h \
../kadas/gui/milx/kadasmilxclient.cpp \
../kadas/gui/milx/kadasmilxinterface.h \
../kadas/gui/milx/kadasmilxlibrary.cpp \
../kadas/gui/milx/kadasmilxclient.h \
../kadas/gui/milx/kadasmilxitem.h \
../kadas/gui/kadasattributetabledialog.h \
../kadas/gui/kadasattributetabledialog.cpp \
../kadas/gui/kadasprojecttemplateselectiondialog.cpp \
../kadas/gui/kadasribbonbutton.h \
../kadas/gui/kadassearchbox.cpp \
../kadas/gui/kadasclipboard.cpp \
../kadas/gui/kadascoordinateinput.cpp \
../kadas/gui/kadastextbrowser.h \
../kadas/gui/kadasitemcontextmenuactions.cpp \
../kadas/gui/kadasribbonbutton.cpp \
../kadas/gui/kadasrichtexteditor.cpp \
../kadas/gui/kadasclipboard.h \
../kadas/gui/kadascatalogbrowser.cpp \
../kadas/gui/kadasprojecttemplateselectiondialog.h \
../kadas/app/kadascrashrpt.cpp \
../kadas/app/kadascanvascontextmenu.h \
../kadas/app/kadaslayoutdesignermanager.cpp \
../kadas/app/kadasmessagelogviewer.h \
../kadas/app/mapgrid/kadasmaptoolmapgrid.cpp \
../kadas/app/mapgrid/kadasmapgridlayer.cpp \
../kadas/app/mapgrid/kadasmaptoolmapgrid.h \
../kadas/app/mapgrid/kadasmapgridwidgetbase.ui \
../kadas/app/mapgrid/kadasmapgridlayer.h \
../kadas/app/kadasgpsintegration.h \
../kadas/app/kadashandlebadlayers.cpp \
../kadas/app/kadaslayoutappmenuprovider.h \
../kadas/app/kadascanvascontextmenu.cpp \
../kadas/app/kml/kadaskmlexportdialog.h \
../kadas/app/kml/kadaskmlexport.h \
../kadas/app/kml/kadaskmlintegration.h \
../kadas/app/kml/kadaskmlexport.cpp \
../kadas/app/kml/kadaskmllabeling.h \
../kadas/app/kml/kadaskmlexportdialog.cpp \
../kadas/app/kml/kadaskmlimport.h \
../kadas/app/kml/kadaskmllabeling.cpp \
../kadas/app/kml/kadaskmlintegration.cpp \
../kadas/app/kml/kadaskmlexportdialogbase.ui \
../kadas/app/kml/kadaskmlimport.cpp \
../kadas/app/kadaslayoutdesignermanager.h \
../kadas/app/kadasmessagelogviewer.cpp \
../kadas/app/kadaslayoutappmenuprovider.cpp \
../kadas/app/kadasmainwindow.cpp \
../kadas/app/kadaslayertreeviewmenuprovider.cpp \
../kadas/app/kadaspythonintegration.h \
../kadas/app/kadasitemlayerproperties.cpp \
../kadas/app/kadasgpxintegration.h \
../kadas/app/kadaspluginmanager.cpp \
../kadas/app/kadasgpxintegration.cpp \
../kadas/app/kadaslayertreemodel.h \
../kadas/app/kadaslayertreemodel.cpp \
../kadas/app/kadaslayoutdesignerdialog.cpp \
../kadas/app/kadashandlebadlayers.h \
../kadas/app/kadasplugininterfaceimpl.cpp \
../kadas/app/kadasmapwidgetmanager.cpp \
../kadas/app/main.cpp \
../kadas/app/kadaspluginmanager.h \
../kadas/app/kadasprojectmigration.h \
../kadas/app/ui/kadaspluginmanager.ui \
../kadas/app/ui/KadasMilxExportDialog.ui \
../kadas/app/ui/kadastopwidget.ui \
../kadas/app/ui/kadaslayoutdesignerbase.ui \
../kadas/app/ui/kadaswindowbase.ui \
../kadas/app/ui/kadasstatuswidget.ui \
../kadas/app/kadasprojectmigration.cpp \
../kadas/app/kadasredliningintegration.cpp \
../kadas/app/iamauth/kadasiamauth.h \
../kadas/app/iamauth/kadasiamauth.cpp \
../kadas/app/kadasmapidentifydialog.cpp \
../kadas/app/kadascrashrpt.h \
../kadas/app/kadasapplication.h \
../kadas/app/bullseye/kadasmaptoolbullseye.cpp \
../kadas/app/bullseye/kadasbullseyelayer.cpp \
../kadas/app/bullseye/kadasmaptoolbullseye.h \
../kadas/app/bullseye/kadasbullseyelayer.h \
../kadas/app/bullseye/kadasbullseyewidgetbase.ui \
../kadas/app/milx/kadasmilxintegration.cpp \
../kadas/app/milx/kadasmilxintegration.h \
../kadas/app/kadasredliningintegration.h \
../kadas/app/guidegrid/kadasguidegridwidgetbase.ui \
../kadas/app/guidegrid/kadasguidegridlayer.h \
../kadas/app/guidegrid/kadasmaptoolguidegrid.h \
../kadas/app/guidegrid/kadasguidegridlayer.cpp \
../kadas/app/guidegrid/kadasmaptoolguidegrid.cpp \
../kadas/app/kadasplugininterfaceimpl.h \
../kadas/app/kadaslayertreeviewmenuprovider.h \
../kadas/app/globe/kadasglobevectorlayerproperties.h \
../kadas/app/globe/kadasglobeinteractionhandlers.h \
../kadas/app/globe/kadasglobebillboardmanager.cpp \
../kadas/app/globe/kadasglobetilesource.h \
../kadas/app/globe/kadasglobedialog.h \
../kadas/app/globe/kadasglobebillboardmanager.h \
../kadas/app/globe/kadasglobeinteractionhandlers.cpp \
../kadas/app/globe/kadasglobevectorlayerproperties.ui \
../kadas/app/globe/kadasglobeintegration.cpp \
../kadas/app/globe/kadasglobeintegration.h \
../kadas/app/globe/kadasglobetilesource.cpp \
../kadas/app/globe/kadasglobewidget.h \
../kadas/app/globe/kadasglobedialog.ui \
../kadas/app/globe/kadasglobevectorlayerproperties.cpp \
../kadas/app/globe/kadasglobeprojectlayermanager.h \
../kadas/app/globe/featuresource/kadasglobefeaturesource.h \
../kadas/app/globe/featuresource/kadasglobefeatureoptions.h \
../kadas/app/globe/featuresource/kadasglobefeaturesource.cpp \
../kadas/app/globe/featuresource/kadasglobefeatureutils.h \
../kadas/app/globe/kadasglobewidget.cpp \
../kadas/app/globe/kadasglobedialog.cpp \
../kadas/app/globe/kadasglobeprojectlayermanager.cpp \
../kadas/app/kadasgpsintegration.cpp \
../kadas/app/kadasmainwindow.h \
../kadas/app/kadaspythonintegration.cpp \
../kadas/app/kadasitemlayerproperties.h \
../kadas/app/kadasmapwidgetmanager.h \
../kadas/app/kadasapplication.cpp \
../kadas/app/kadasmapidentifydialog.h \
../kadas/app/kadaslayoutdesignerdialog.h \

TRANSLATIONS += \
Kadas_de.ts \
Kadas_fr.ts \
Kadas_it.ts \
