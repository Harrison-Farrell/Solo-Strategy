// -----------------------------------------------------------------------------
// Author:      Harrison Farrell
// Project:     Solo-Strategy Trading System
// Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
//
// Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
// This program is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
// -----------------------------------------------------------------------------

// 3rd party libraries
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QLineSeries* buyLine = new QLineSeries();
    QLineSeries* sellLine = new QLineSeries();

    // Buy Side (Bids): Volume increases as price decreases
    // Logic: Add two points per level to create the "step"
    *buyLine << QPointF(100, 0) << QPointF(100, 50) << QPointF(99, 50)
             << QPointF(99, 120) << QPointF(98, 120) << QPointF(98, 200)
             << QPointF(97, 200) << QPointF(97, 350);

    // Sell Side (Asks): Volume increases as price increases
    *sellLine << QPointF(102, 0) << QPointF(102, 50) << QPointF(103, 50)
              << QPointF(103, 120) << QPointF(104, 120) << QPointF(104, 200)
              << QPointF(105, 200) << QPointF(105, 350);

    QAreaSeries* buyArea = new QAreaSeries(buyLine);
    buyArea->setName("Bids (Buy)");
    buyArea->setBrush(QColor(0, 200, 80, 150));  // Semi-transparent Green
    buyArea->setPen(QPen(QColor(0, 200, 80), 2));

    QAreaSeries* sellArea = new QAreaSeries(sellLine);
    sellArea->setName("Asks (Sell)");
    sellArea->setBrush(QColor(230, 50, 50, 150));  // Semi-transparent Red
    sellArea->setPen(QPen(QColor(230, 50, 50), 2));

    QChart* chart = new QChart();
    chart->addSeries(buyArea);
    chart->addSeries(sellArea);
    chart->setTitle("Market Depth: AAPL");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("Price ($)");
    axisX->setLabelFormat("%.2f");
    axisX->setRange(97, 105);
    chart->addAxis(axisX, Qt::AlignBottom);
    buyArea->attachAxis(axisX);
    sellArea->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Cumulative Volume");
    chart->addAxis(axisY, Qt::AlignLeft);
    buyArea->attachAxis(axisY);
    sellArea->attachAxis(axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(800, 500);
    window.setWindowTitle("Market Depth Chart");
    window.show();

    return a.exec();
}