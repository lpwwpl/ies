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


#ifndef TableItemDelegate_H
#define TableItemDelegate_H

#include <QItemDelegate>
#include <QComboBox>
#include <QStyleOptionViewItem>

#include <QStyledItemDelegate>

    class ComboDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        ComboDelegate(QObject* parent = 0);

        void setItems(QStringList items);

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const  QModelIndex& index) const;
    Q_SIGNALS:
        //void signalEndEdit();
        void editingFinished();
    private:
        QStringList m_sItemList;    //下拉框列表项
    };

    /*
     * DoubleSpinBox
     */
    class SpinBoxDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        SpinBoxDelegate(QObject* parent = 0);

        void setRange(double min, double max);

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const  QModelIndex& index) const;
 
    Q_SIGNALS:
        //void signalEndEdit();
        void editingFinished();
    private:
        double minValue;        //最小值
        double maxValue;        //最大值
    };

    class DoubleLineEditDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        DoubleLineEditDelegate(QObject* parent = 0);

        void setRange(double bottom, double top, int decimals = 0);

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const  QModelIndex& index) const;
    Q_SIGNALS:
        //void signalEndEdit();
        void editingFinished();
    private:
        double min;        //最小值
        double max;        //最大值
        int dec;        //小数位数
    };

    class IntLineEditDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        IntLineEditDelegate(QObject* parent = 0);

        void setRange(int bottom, int top);

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const  QModelIndex& index) const;
    Q_SIGNALS:
        //void signalEndEdit();
        void editingFinished();
    private:
        int min;        //最小值
        int max;        //最大值
    };


    class LineEditDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        LineEditDelegate(QObject* parent = 0);
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const  QModelIndex& index) const;
    Q_SIGNALS:
        //void signalEndEdit();
        void editingFinished();
    public:
        QString oldStr;
    };
    class DoubleItemDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        explicit DoubleItemDelegate(QObject* parent = nullptr);

        void setRange(double min, double max, int decimals = 6);

        QWidget* createEditor(QWidget* parent,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;

        void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        void setModelData(QWidget* editor,
            QAbstractItemModel* model,
            const QModelIndex& index) const override;

        void updateEditorGeometry(QWidget* editor,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;
    Q_SIGNALS:
        //void signalEndEdit();
        void editingFinished();
    private:
        double minValue;
        double maxValue;
        int decimalPlaces;
    };


#endif // GUI_TASKVIEW_TASKAPPERANCE_H
