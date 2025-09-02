#include "piechart.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QtMath>
#include <QDebug>

PieChart::PieChart(QWidget *parent)
    : QWidget(parent)
    , m_showPercentages(true)
    , m_showLegend(true)
    , m_animated(true)
    , m_animationProgress(0.0)
    , m_margin(20)
    , m_legendWidth(150)
    , m_titleHeight(40)
{
    // Setup fonts
    m_titleFont = QFont("Arial", 14, QFont::Bold);
    m_labelFont = QFont("Arial", 9, QFont::Normal);
    m_legendFont = QFont("Arial", 9, QFont::Normal);
    
    // Setup animation
    m_animation = new QPropertyAnimation(this, "animationProgress");
    m_animation->setDuration(1500);
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    
    setMinimumSize(300, 250);
}

void PieChart::addSlice(const QString& label, double value, const QColor& color)
{
    m_slices.append(PieSlice(label, value, color));
    calculateAngles();
    
    if (m_animated) {
        m_animation->start();
    } else {
        m_animationProgress = 1.0;
        update();
    }
}

void PieChart::clearSlices()
{
    m_slices.clear();
    m_animationProgress = 0.0;
    update();
}

void PieChart::setTitle(const QString& title)
{
    m_title = title;
    update();
}

void PieChart::setShowPercentages(bool show)
{
    m_showPercentages = show;
    update();
}

void PieChart::setShowLegend(bool show)
{
    m_showLegend = show;
    update();
}

void PieChart::setAnimated(bool animated)
{
    m_animated = animated;
}

QSize PieChart::sizeHint() const
{
    return QSize(400, 300);
}

QSize PieChart::minimumSizeHint() const
{
    return QSize(250, 200);
}

void PieChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Clear background
    painter.fillRect(rect(), QColor(255, 255, 255));
    
    if (m_slices.isEmpty()) {
        return;
    }
    
    drawTitle(painter);
    drawPieChart(painter);
    
    if (m_showLegend) {
        drawLegend(painter);
    }
}

void PieChart::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    calculateAngles();
}

void PieChart::updateAnimation()
{
    update();
}

void PieChart::calculateAngles()
{
    if (m_slices.isEmpty()) {
        return;
    }
    
    // Calculate total value
    double total = 0.0;
    for (const auto& slice : m_slices) {
        total += slice.value;
    }
    
    if (total <= 0) {
        return;
    }
    
    // Calculate angles and percentages
    int currentAngle = 0; // Start from top (12 o'clock)
    for (auto& slice : m_slices) {
        slice.percentage = (slice.value / total) * 100.0;
        slice.startAngle = currentAngle;
        slice.spanAngle = static_cast<int>((slice.value / total) * 360.0 * 16); // Qt uses 16ths of degrees
        currentAngle += slice.spanAngle;
    }
}

void PieChart::drawPieChart(QPainter& painter)
{
    QRect pieRect = getPieRect();
    
    if (pieRect.width() <= 0 || pieRect.height() <= 0) {
        return;
    }
    
    // Draw pie slices
    for (const auto& slice : m_slices) {
        // Apply animation progress
        int animatedSpan = static_cast<int>(slice.spanAngle * m_animationProgress);
        
        if (animatedSpan > 0) {
            painter.setBrush(slice.color);
            painter.setPen(QPen(Qt::white, 2));
            painter.drawPie(pieRect, slice.startAngle, animatedSpan);
            
            // Draw percentage labels if enabled
            if (m_showPercentages && m_animationProgress > 0.7) {
                double midAngle = (slice.startAngle + animatedSpan / 2.0) / 16.0; // Convert to degrees
                double radians = qDegreesToRadians(midAngle);
                
                // Calculate label position
                int radius = qMin(pieRect.width(), pieRect.height()) / 2;
                int labelRadius = radius * 0.7; // Position labels at 70% of radius
                
                QPoint center = pieRect.center();
                int x = center.x() + static_cast<int>(labelRadius * qSin(radians));
                int y = center.y() - static_cast<int>(labelRadius * qCos(radians));
                
                // Draw percentage text
                painter.setFont(m_labelFont);
                painter.setPen(Qt::white);
                
                QString percentText = QString("%1%").arg(slice.percentage, 0, 'f', 1);
                QFontMetrics fm(m_labelFont);
                QRect textRect = fm.boundingRect(percentText);
                textRect.moveCenter(QPoint(x, y));
                
                // Draw background for better readability
                painter.setBrush(QColor(0, 0, 0, 100));
                painter.setPen(Qt::NoPen);
                painter.drawRoundedRect(textRect.adjusted(-4, -2, 4, 2), 3, 3);
                
                // Draw text
                painter.setPen(Qt::white);
                painter.setBrush(Qt::NoBrush);
                painter.drawText(textRect, Qt::AlignCenter, percentText);
            }
        }
    }
}

void PieChart::drawLegend(QPainter& painter)
{
    QRect legendRect = getLegendRect();

    if (legendRect.width() <= 0 || legendRect.height() <= 0) {
        return;
    }

    painter.setFont(m_legendFont);
    QFontMetrics fm(m_legendFont);

    int y = legendRect.top();
    int lineHeight = fm.height() + 4;
    int colorBoxSize = 12;

    for (const auto& slice : m_slices) {
        if (y + lineHeight > legendRect.bottom()) {
            break; // Not enough space
        }

        // Draw color box
        QRect colorRect(legendRect.left(), y + 2, colorBoxSize, colorBoxSize);
        painter.setBrush(slice.color);
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawRoundedRect(colorRect, 2, 2);

        // Draw label and percentage
        QString text = QString("%1 (%2%)").arg(slice.label).arg(slice.percentage, 0, 'f', 1);
        QRect textRect(legendRect.left() + colorBoxSize + 8, y,
                      legendRect.width() - colorBoxSize - 8, lineHeight);

        painter.setPen(QColor(60, 60, 60));
        painter.setBrush(Qt::NoBrush);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

        y += lineHeight;
    }
}

void PieChart::drawTitle(QPainter& painter)
{
    if (m_title.isEmpty()) {
        return;
    }

    painter.setFont(m_titleFont);
    painter.setPen(QColor(40, 40, 40));

    QRect titleRect(m_margin, m_margin, width() - 2 * m_margin, m_titleHeight);
    painter.drawText(titleRect, Qt::AlignCenter, m_title);
}

QRect PieChart::getPieRect() const
{
    int availableWidth = width() - 2 * m_margin;
    int availableHeight = height() - 2 * m_margin - m_titleHeight;

    if (m_showLegend) {
        availableWidth -= m_legendWidth;
    }

    int size = qMin(availableWidth, availableHeight);
    if (size <= 0) {
        return QRect();
    }

    int x = m_margin + (availableWidth - size) / 2;
    int y = m_margin + m_titleHeight + (availableHeight - size) / 2;

    return QRect(x, y, size, size);
}

QRect PieChart::getLegendRect() const
{
    if (!m_showLegend) {
        return QRect();
    }

    int x = width() - m_margin - m_legendWidth;
    int y = m_margin + m_titleHeight;
    int h = height() - 2 * m_margin - m_titleHeight;

    return QRect(x, y, m_legendWidth, h);
}

void PieChart::setAnimationProgress(double progress)
{
    m_animationProgress = qBound(0.0, progress, 1.0);
    update();
}
