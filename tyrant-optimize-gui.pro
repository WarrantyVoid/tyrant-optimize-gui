######################################################################
# Automatically generated by qmake (2.01a) Sa 29. Dez 19:30:17 2012
######################################################################

QT += core widgets network xml multimedia

TEMPLATE = app 

DEPENDPATH += .

INCLUDEPATH += .

# Input
HEADERS += \
    CMainWindow.h \
    CDeckWidget.h \
    CWinLabel.h \
    xml/CQuestsXmlParser.h \
    xml/CAchievementsXmlParser.h \
    xml/CMissionsXmlParser.h \
    xml/CRaidsXmlParser.h \
    xml/CCardsXmlParser.h \
    CCardFilterWidget.h \
    CMultiDeckEditorWidget.h \
    CMultiDeckWidget.h \
    model/CAchievement.h \
    model/CCard.h \
    model/CDeck.h \
    model/CSkill.h \
    model/CBattleground.h \
    model/CDeckTable.h \
    model/CCardTable.h \
    process/CProcessParameters.h \
    process/IProcessWrapper.h \
    process/CTyrantOptimizeWrapper.h \
    CCardSearchWidget.h \
    ICardCheck.h \
    CDeckManagementWidget.h \
    CCardLabel.h \
    CDeckInput.h \
    CDeckSaveWidget.h \
    CGlobalConfig.h \
    CCardLabelNexus.h \
    CStatusWidget.h \
    process/CProcessPriority.h \
    filter/CCardFilter.h \
    filter/CCardFilterParameters.h \
    download/CDownload.h \
    download/CPictureDownload.h \
    filter/CCardSearchParameters.h \
    CVersion.h \
    model/CCardSkill.h \
    model/CardTypes.h \
    xml/CXmlSaxValue.h
	   
FORMS += \
    MainWindow.ui \
    DeckWidget.ui \
    CardFilterWidget.ui \
    MultiDeckEditorWidget.ui \
    CardSearchWidget.ui \
    DeckManagementWidget.ui \
    DeckSaveWidget.ui

SOURCES += \
    CMainWindow.cpp \
    main.cpp \
    CDeckWidget.cpp \
    CWinLabel.cpp \
    xml/CQuestsXmlParser.cpp \
    xml/CMissionsXmlParser.cpp \
    xml/CAchievementsXmlParser.cpp \
    xml/CCardsXmlParser.cpp \
    xml/CRaidsXmlParser.cpp \
    CCardFilterWidget.cpp \
    CMultiDeckEditorWidget.cpp \
    CMultiDeckWidget.cpp \
    model/CAchievement.cpp \
    model/CCard.cpp \
    model/CDeck.cpp \
    model/CSkill.cpp \
    model/CBattleground.cpp \
    model/CCardTable.cpp \
    model/CDeckTable.cpp \
    process/CTyrantOptimizeWrapper.cpp \
    process/CProcessParameters.cpp \
    CCardSearchWidget.cpp \
    CDeckManagementWidget.cpp \
    CCardLabel.cpp \
    CDeckInput.cpp \
    CDeckSaveWidget.cpp \
    CGlobalConfig.cpp \
    CCardLabelNexus.cpp \
    CStatusWidget.cpp \
    process/CProcessPriority.cpp \
    filter/CCardFilter.cpp \
    download/CDownload.cpp \
    download/CPictureDownload.cpp \
    filter/CCardSearchParameters.cpp \
    filter/CCardFilterParameters.cpp \
    CVersion.cpp \
    model/CCardSkill.cpp

RESOURCES += main.qrc
RC_FILE    = main.rc
