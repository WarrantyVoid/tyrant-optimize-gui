######################################################################
# Automatically generated by qmake (2.01a) Sa 29. Dez 19:30:17 2012
######################################################################

QT += core network xml

TEMPLATE = app

TARGET = 

DEPENDPATH += .

INCLUDEPATH += .

# Input
HEADERS += \
    CMainWindow.h \
    CPictureDownload.h \
    CDownload.h \
    CDeckWidget.h \
    CWinLabel.h \
    xml/CQuestsXmlParser.h \
    xml/CAchievementsXmlParser.h \
    xml/CMissionsXmlParser.h \
    xml/CRaidsXmlParser.h \
    xml/CCardsXmlParser.h \
    CCardFilterParameters.h \
    CCardFilter.h \
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
    CCardSearchParameters.h \
    CCardSearchWidget.h \
    ICardCheck.h \
    CDeckManagementWidget.h \
    CCardLabel.h \
    CDeckInput.h \
    CDeckSaveWidget.h \
    CGlobalConfig.h \
    CCardLabelNexus.h
	   
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
    CPictureDownload.cpp \
    main.cpp \
    CDownload.cpp \
    CDeckWidget.cpp \
    CWinLabel.cpp \
    xml/CQuestsXmlParser.cpp \
    xml/CMissionsXmlParser.cpp \
    xml/CAchievementsXmlParser.cpp \
    xml/CCardsXmlParser.cpp \
    xml/CRaidsXmlParser.cpp \
    CCardFilterParameters.cpp \
    CCardFilter.cpp \
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
    CCardSearchParameters.cpp \
    CCardSearchWidget.cpp \
    CDeckManagementWidget.cpp \
    CCardLabel.cpp \
    CDeckInput.cpp \
    CDeckSaveWidget.cpp \
    CGlobalConfig.cpp \
    CCardLabelNexus.cpp

RESOURCES += main.qrc
RC_FILE    = main.rc
