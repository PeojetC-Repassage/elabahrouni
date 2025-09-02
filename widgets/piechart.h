#ifndef PIECHART_H
#define PIECHART_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QRect>
#include <QPoint>
#include <QList>
#include <QPair>
#include <QString>
#include <QTimer>
#include <QPropertyAnimation>

struct PieSlice {
    QString label;
    double value;
    QColor color;
    double percentage;
    int startAngle;
    int spanAngle;
    
    PieSlice(const QString& l, double v, const QColor& c) 
        : label(l), value(v), color(c), percentage(0), startAngle(0), spanAngle(0) {}
};

class PieChart : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double animationProgress READ animationProgress WRITE setAnimationProgress)

public:
    explicit PieChart(QWidget *parent = nullptr);
    
    void addSlice(const QString& label, double value, const QColor& color);
    void clearSlices();
    void setTitle(const QString& title);
    void setShowPercentages(bool show);
    void setShowLegend(bool show);
    void setAnimated(bool animated);
    
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateAnimation();

private:
    void calculateAngles();
    void drawPieChart(QPainter& painter);
    void drawLegend(QPainter& painter);
    void drawTitle(QPainter& painter);
    QRect getPieRect() const;
    QRect getLegendRect() const;
    
    double animationProgress() const { return m_animationProgress; }
    void setAnimationProgress(double progress);

private:
    QList<PieSlice> m_slices;
    QString m_title;
    bool m_showPercentages;
    bool m_showLegend;
    bool m_animated;
    double m_animationProgress;
    QPropertyAnimation* m_animation;
    
    // Styling
    QFont m_titleFont;
    QFont m_labelFont;
    QFont m_legendFont;
    int m_margin;
    int m_legendWidth;
    int m_titleHeight;
};

#endif // PIECHART_H
