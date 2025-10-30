/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "TableItemDelegate.h"

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <qdebug.h>

//using namespace Gui;


ComboDelegate::ComboDelegate(QObject* parent) :
    QItemDelegate(parent)
{
}

void ComboDelegate::setItems(QStringList items)
{
    m_sItemList = items;
}

QWidget* ComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&/*option*/, const QModelIndex&/*index*/) const
{
    QComboBox* editor = new QComboBox(parent);
    editor->installEventFilter(this->parent());
    editor->addItems(m_sItemList);
    //editor->installEventFilter(const_cast<ComboDelegate*>(this));
    return editor;
}

void ComboDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString str = index.model()->data(index).toString();

    QComboBox* box = static_cast<QComboBox*>(editor);
    int i = box->findText(str);
    box->setCurrentIndex(i);
}

void ComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);
    QString str = box->currentText();
    model->setData(index, str);

}

void ComboDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&/*index*/) const
{
    editor->setGeometry(option.rect);
}

/****************************************************
 * SpinBoxDelegate
 ****************************************************/
SpinBoxDelegate::SpinBoxDelegate(QObject* parent) :
    QItemDelegate(parent)
{
    //初始化最大最小值
    minValue = -65536.00;
    maxValue = 65536.00;
}

void SpinBoxDelegate::setRange(double min, double max)
{
    minValue = min;
    maxValue = max;
}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    connect(editor, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    editor->installEventFilter(this->parent());
    editor->setMinimum(minValue);
    editor->setMaximum(maxValue);
    editor->setSingleStep(1);
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    double value = index.model()->data(index, Qt::DisplayRole).toDouble();
    QDoubleSpinBox* spinBox = static_cast <QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QDoubleSpinBox* spinBox = static_cast <QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();
    model->setData(index, value, Qt::DisplayRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}

LineEditDelegate::LineEditDelegate(QObject* parent) :
    QItemDelegate(parent)
{

}


QWidget* LineEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&/*option*/, const QModelIndex&/*index*/) const
{
    QLineEdit* editor = new QLineEdit(parent);
    editor->installEventFilter(this->parent());
    return editor;
}

void LineEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString str = index.model()->data(index).toString();
    
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    
    lineEdit->setText(str);
}

void LineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString str = lineEdit->text();

    model->setData(index, str);
}

void LineEditDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&/*index*/) const
{
    editor->setGeometry(option.rect);
}



DoubleLineEditDelegate::DoubleLineEditDelegate(QObject* parent) :
    QItemDelegate(parent)
{
    min = -65535;
    max = 65535;
}

void DoubleLineEditDelegate::setRange(double bottom, double top, int decimals)
{
    min = bottom;
    max = top;
    dec = decimals;
}

QWidget* DoubleLineEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&/*option*/, const QModelIndex&/*index*/) const
{
    QLineEdit* editor = new QLineEdit(parent);
    editor->installEventFilter(this->parent());
    editor->setValidator(new QDoubleValidator(min, max, dec));
    return editor;
}

void DoubleLineEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString str = index.model()->data(index).toString();

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(str);
}

void DoubleLineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString str = lineEdit->text();
    model->setData(index, str);
}

void DoubleLineEditDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&/*index*/) const
{
    editor->setGeometry(option.rect);
}



IntLineEditDelegate::IntLineEditDelegate(QObject* parent) :
    QItemDelegate(parent)
{
    min = 0;
    max = 1;
}
void IntLineEditDelegate::setRange(int bottom, int top)
{
    min = bottom;
    max = top;
}

QWidget* IntLineEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&/*option*/, const QModelIndex&/*index*/) const
{
    
    QLineEdit* editor = new QLineEdit(parent);
    editor->installEventFilter(this->parent());
    editor->setValidator(new QIntValidator(min, max));
    return editor;
}

void IntLineEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString str = index.model()->data(index).toString();

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(str);
}

void IntLineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString str = lineEdit->text();
    model->setData(index, str);
}

void IntLineEditDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&/*index*/) const
{
    editor->setGeometry(option.rect);
}


DoubleItemDelegate::DoubleItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
    minValue(0.0),
    maxValue(1000000.0),
    decimalPlaces(6)
{
}

void DoubleItemDelegate::setRange(double min, double max, int decimals)
{
    minValue = min;
    maxValue = max;
    decimalPlaces = decimals;
}

QWidget* DoubleItemDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QLineEdit* editor = new QLineEdit(parent);

    // 设置严格的验证器
    QDoubleValidator* validator = new QDoubleValidator(minValue, maxValue, decimalPlaces, editor);
    validator->setNotation(QDoubleValidator::StandardNotation);
    editor->setValidator(validator);

    connect(editor, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    // 设置输入提示
    //editor->setPlaceholderText(QString("范围: %1 到 %2").arg(minValue).arg(maxValue));

    return editor;
}

void DoubleItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit) {
        lineEdit->setText(value);
        lineEdit->selectAll(); // 全选以便直接替换
    }
}

void DoubleItemDelegate::setModelData(QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit) {
        QString text = lineEdit->text().trimmed();

        // 额外验证
        bool ok;
        double value = text.toDouble(&ok);

        if (ok && value >= minValue && value <= maxValue) {
            model->setData(index, text, Qt::DisplayRole);
        }
        //else if (text.isEmpty()) {
        //    model->setData(index, "0.0", Qt::DisplayRole); // 空值设为0
        //}       
        // 否则保持原值（验证器应该已经阻止了非法输入）
    }
}

void DoubleItemDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}