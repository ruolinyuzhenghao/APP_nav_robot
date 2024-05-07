QT       += core gui statemachine websockets
QT       += charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    battery.cpp \
    battery_warn.cpp \
    components/lib/qtmaterialcheckable.cpp \
    components/lib/qtmaterialcheckable_internal.cpp \
    components/lib/qtmaterialoverlaywidget.cpp \
    components/lib/qtmaterialripple.cpp \
    components/lib/qtmaterialrippleoverlay.cpp \
    components/lib/qtmaterialstatetransition.cpp \
    components/lib/qtmaterialstyle.cpp \
    components/lib/qtmaterialtheme.cpp \
    components/qtmaterialappbar.cpp \
    components/qtmaterialautocomplete.cpp \
    components/qtmaterialautocomplete_internal.cpp \
    components/qtmaterialavatar.cpp \
    components/qtmaterialbadge.cpp \
    components/qtmaterialcheckbox.cpp \
    components/qtmaterialcircularprogress.cpp \
    components/qtmaterialcircularprogress_internal.cpp \
    components/qtmaterialdialog.cpp \
    components/qtmaterialdialog_internal.cpp \
    components/qtmaterialdrawer.cpp \
    components/qtmaterialdrawer_internal.cpp \
    components/qtmaterialfab.cpp \
    components/qtmaterialflatbutton.cpp \
    components/qtmaterialflatbutton_internal.cpp \
    components/qtmaterialiconbutton.cpp \
    components/qtmateriallist.cpp \
    components/qtmateriallistitem.cpp \
    components/qtmaterialmenu.cpp \
    components/qtmaterialmenu_internal.cpp \
    components/qtmaterialpaper.cpp \
    components/qtmaterialprogress.cpp \
    components/qtmaterialprogress_internal.cpp \
    components/qtmaterialradiobutton.cpp \
    components/qtmaterialraisedbutton.cpp \
    components/qtmaterialscrollbar.cpp \
    components/qtmaterialscrollbar_internal.cpp \
    components/qtmaterialslider.cpp \
    components/qtmaterialslider_internal.cpp \
    components/qtmaterialsnackbar.cpp \
    components/qtmaterialsnackbar_internal.cpp \
    components/qtmaterialtable.cpp \
    components/qtmaterialtabs.cpp \
    components/qtmaterialtabs_internal.cpp \
    components/qtmaterialtextfield.cpp \
    components/qtmaterialtextfield_internal.cpp \
    components/qtmaterialtoggle.cpp \
    components/qtmaterialtoggle_internal.cpp \
    customrocker.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    battery.h \
    battery_warn.h \
    components/lib/qtmaterialcheckable.h \
    components/lib/qtmaterialcheckable_internal.h \
    components/lib/qtmaterialcheckable_p.h \
    components/lib/qtmaterialoverlaywidget.h \
    components/lib/qtmaterialripple.h \
    components/lib/qtmaterialrippleoverlay.h \
    components/lib/qtmaterialstatetransition.h \
    components/lib/qtmaterialstatetransitionevent.h \
    components/lib/qtmaterialstyle.h \
    components/lib/qtmaterialstyle_p.h \
    components/lib/qtmaterialtheme.h \
    components/lib/qtmaterialtheme_p.h \
    components/qtmaterialappbar.h \
    components/qtmaterialappbar_p.h \
    components/qtmaterialautocomplete.h \
    components/qtmaterialautocomplete_internal.h \
    components/qtmaterialautocomplete_p.h \
    qtmaterialavatar.h \
    components/qtmaterialavatar_p.h \
    components/qtmaterialbadge.h \
    components/qtmaterialbadge_p.h \
    qtmaterialcheckbox.h \
    components/qtmaterialcheckbox_p.h \
    qtmaterialcircularprogress.h \
    components/qtmaterialcircularprogress_internal.h \
    components/qtmaterialcircularprogress_p.h \
    qtmaterialdialog.h \
    components/qtmaterialdialog_internal.h \
    components/qtmaterialdialog_p.h \
    components/qtmaterialdrawer.h \
    components/qtmaterialdrawer_internal.h \
    components/qtmaterialdrawer_p.h \
    components/qtmaterialfab.h \
    components/qtmaterialfab_p.h \
    components/qtmaterialflatbutton.h \
    components/qtmaterialflatbutton_internal.h \
    components/qtmaterialflatbutton_p.h \
    components/qtmaterialiconbutton.h \
    components/qtmaterialiconbutton_p.h \
    components/qtmateriallist.h \
    components/qtmateriallist_p.h \
    components/qtmateriallistitem.h \
    components/qtmateriallistitem_p.h \
    components/qtmaterialmenu.h \
    components/qtmaterialmenu_internal.h \
    components/qtmaterialmenu_p.h \
    components/qtmaterialpaper.h \
    components/qtmaterialpaper_p.h \
    qtmaterialprogress.h \
    components/qtmaterialprogress_internal.h \
    components/qtmaterialprogress_p.h \
    qtmaterialradiobutton.h \
    components/qtmaterialradiobutton_p.h \
    components/qtmaterialraisedbutton.h \
    components/qtmaterialraisedbutton_p.h \
    components/qtmaterialscrollbar.h \
    components/qtmaterialscrollbar_internal.h \
    components/qtmaterialscrollbar_p.h \
    qtmaterialslider.h \
    components/qtmaterialslider_internal.h \
    components/qtmaterialslider_p.h \
    components/qtmaterialsnackbar.h \
    components/qtmaterialsnackbar_internal.h \
    components/qtmaterialsnackbar_p.h \
    components/qtmaterialtable.h \
    components/qtmaterialtable_p.h \
    customrocker.h \
    qtmaterialraisedbutton.h \
    qtmaterialtabs.h \
    components/qtmaterialtabs_internal.h \
    components/qtmaterialtabs_p.h \
    qtmaterialtextfield.h \
    components/qtmaterialtextfield_internal.h \
    components/qtmaterialtextfield_p.h \
    qtmaterialtoggle.h \
    components/qtmaterialtoggle_internal.h \
    components/qtmaterialtoggle_p.h \
    mainwindow.h

FORMS += \
    battery_warn.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc \
    rec.qrc
