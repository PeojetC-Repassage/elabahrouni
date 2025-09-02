/****************************************************************************
** Meta object code from reading C++ file 'commande.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../models/commande.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'commande.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSCommandeENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCommandeENDCLASS = QtMocHelpers::stringData(
    "Commande",
    "dataChanged",
    "",
    "Statut",
    "EN_ATTENTE",
    "CONFIRMEE",
    "EN_PREPARATION",
    "EN_TRANSIT",
    "LIVREE",
    "ANNULEE",
    "Priorite",
    "BASSE",
    "NORMALE",
    "HAUTE",
    "URGENTE"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCommandeENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       2,   21, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   20,    2, 0x06,    3 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // enums: name, alias, flags, count, data
       3,    3, 0x0,    6,   31,
      10,   10, 0x0,    4,   43,

 // enum data: key, value
       4, uint(Commande::EN_ATTENTE),
       5, uint(Commande::CONFIRMEE),
       6, uint(Commande::EN_PREPARATION),
       7, uint(Commande::EN_TRANSIT),
       8, uint(Commande::LIVREE),
       9, uint(Commande::ANNULEE),
      11, uint(Commande::BASSE),
      12, uint(Commande::NORMALE),
      13, uint(Commande::HAUTE),
      14, uint(Commande::URGENTE),

       0        // eod
};

Q_CONSTINIT const QMetaObject Commande::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSCommandeENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCommandeENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCommandeENDCLASS_t,
        // enum 'Statut'
        QtPrivate::TypeAndForceComplete<Commande::Statut, std::true_type>,
        // enum 'Priorite'
        QtPrivate::TypeAndForceComplete<Commande::Priorite, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Commande, std::true_type>,
        // method 'dataChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Commande::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Commande *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Commande::*)();
            if (_t _q_method = &Commande::dataChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *Commande::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Commande::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCommandeENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Commande::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Commande::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
