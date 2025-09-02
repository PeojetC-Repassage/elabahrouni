/****************************************************************************
** Meta object code from reading C++ file 'clientview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../views/clientview.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSClientViewENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSClientViewENDCLASS = QtMocHelpers::stringData(
    "ClientView",
    "refreshData",
    "",
    "onAddClient",
    "onEditClient",
    "onDeleteClient",
    "onSaveClient",
    "onCancelEdit",
    "onSearchClients",
    "onClearSearch",
    "onSortChanged",
    "onClientSelectionChanged",
    "onClientDoubleClicked",
    "row",
    "column",
    "onClientCreated",
    "Client*",
    "client",
    "onClientUpdated",
    "onClientDeleted",
    "clientId",
    "onControllerError",
    "message"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSClientViewENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  104,    2, 0x0a,    1 /* Public */,
       3,    0,  105,    2, 0x08,    2 /* Private */,
       4,    0,  106,    2, 0x08,    3 /* Private */,
       5,    0,  107,    2, 0x08,    4 /* Private */,
       6,    0,  108,    2, 0x08,    5 /* Private */,
       7,    0,  109,    2, 0x08,    6 /* Private */,
       8,    0,  110,    2, 0x08,    7 /* Private */,
       9,    0,  111,    2, 0x08,    8 /* Private */,
      10,    0,  112,    2, 0x08,    9 /* Private */,
      11,    0,  113,    2, 0x08,   10 /* Private */,
      12,    2,  114,    2, 0x08,   11 /* Private */,
      15,    1,  119,    2, 0x08,   14 /* Private */,
      18,    1,  122,    2, 0x08,   16 /* Private */,
      19,    1,  125,    2, 0x08,   18 /* Private */,
      21,    1,  128,    2, 0x08,   20 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   13,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::QString,   22,

       0        // eod
};

Q_CONSTINIT const QMetaObject ClientView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSClientViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSClientViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSClientViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ClientView, std::true_type>,
        // method 'refreshData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddClient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditClient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteClient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveClient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancelEdit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchClients'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClearSearch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSortChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClientSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClientDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onClientCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Client *, std::false_type>,
        // method 'onClientUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Client *, std::false_type>,
        // method 'onClientDeleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onControllerError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ClientView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ClientView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->refreshData(); break;
        case 1: _t->onAddClient(); break;
        case 2: _t->onEditClient(); break;
        case 3: _t->onDeleteClient(); break;
        case 4: _t->onSaveClient(); break;
        case 5: _t->onCancelEdit(); break;
        case 6: _t->onSearchClients(); break;
        case 7: _t->onClearSearch(); break;
        case 8: _t->onSortChanged(); break;
        case 9: _t->onClientSelectionChanged(); break;
        case 10: _t->onClientDoubleClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 11: _t->onClientCreated((*reinterpret_cast< std::add_pointer_t<Client*>>(_a[1]))); break;
        case 12: _t->onClientUpdated((*reinterpret_cast< std::add_pointer_t<Client*>>(_a[1]))); break;
        case 13: _t->onClientDeleted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->onControllerError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Client* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Client* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *ClientView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClientView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSClientViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ClientView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
