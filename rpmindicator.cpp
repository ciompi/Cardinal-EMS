//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EngineMonitor, a graphical gauge to monitor an aircraft's engine     //
// Copyright (C) 2012 Tobias Rad                                        //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not, see <http://www.gnu.org/licenses/>. //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "rpmindicator.h"

RpmIndicator::RpmIndicator(QGraphicsObject *parent) : QGraphicsObject(parent)
  , minValue(0.0)
  , maxValue(0.0)
  , currentValue(0.0)
  , whiteGreenBorder(0.0)
  , greenRedBorder(0.0)
  , startAngle(0.0)
  , spanAngle(0.0)
  , gaugeSettings("./gaugeSettings.ini", QSettings::IniFormat)
{
    isWarmup=true;
}


QRectF RpmIndicator::boundingRect() const
{
	return QRectF(-200.0, -140.0, 400.0, 280.0);
}

void RpmIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

    painter->setPen(QPen(Qt::green, 0));
    painter->drawRect(QRectF(-200.0, -140.0, 400.0, 280.0));

	//Draw the arc
	QRectF circle = QRectF(-130.0, -130.0, 260.0, 260.0);
    //Calculate angles for white and red arc part
	double whiteGreenAngle = calculateLocalValue(whiteGreenBorder);
	double greenRedAngle = calculateLocalValue(greenRedBorder);
    double redYellowAngleWarmup = calculateLocalValue(redYellowBorderWarmup);
    double yellowGreenAngleWarmup = calculateLocalValue(yellowGreenBorderWarmup);
    double greenYellowAngleWarmup = calculateLocalValue(greenYellowBorderWarmup);
    double yellowRedAngleWarmup = calculateLocalValue(yellowRedBorderWarmup);
    double redYellowAngle = calculateLocalValue(redYellowBorder);
    double yellowGreenAngle = calculateLocalValue(yellowGreenBorder);
    double greenYellowAngle = calculateLocalValue(greenYellowBorder);
    double yellowRedAngle = calculateLocalValue(yellowRedBorder);

    //Draw the green basis
    painter->setPen(QPen(Qt::green, 0));
    painter->setBrush(Qt::green);
    painter->drawPie(circle, startAngle*16.0, -spanAngle*16.0);
    if (whiteGreenAngle != 0)
    {
        //Draw the white part
        painter->setPen(QPen(Qt::white, 0));
        painter->setBrush(Qt::white);
        painter->drawPie(circle, startAngle*16.0, -fabs(whiteGreenAngle-startAngle)*16.0);
    }

    if (isWarmup) {
        //Draw the upper red part
        painter->setPen(QPen(Qt::red, 0));
        painter->setBrush(Qt::red);
        painter->drawPie(circle, yellowRedAngleWarmup*16.0, -fabs(startAngle-spanAngle-yellowRedAngleWarmup)*16.0);

        //Draw the lower yellow part
        painter->setPen(QPen(Qt::yellow, 0));
        painter->setBrush(Qt::yellow);
        painter->drawPie(circle, redYellowAngleWarmup*16.0, -fabs(yellowGreenAngle-redYellowAngleWarmup)*16.0);

        //Draw the upper yellow part
        painter->setPen(QPen(Qt::yellow, 0));
        painter->setBrush(Qt::yellow);
        painter->drawPie(circle, greenYellowAngleWarmup*16.0, -fabs(yellowRedAngleWarmup-greenYellowAngleWarmup)*16.0);

        //Draw the lower red part
        painter->setPen(QPen(Qt::red, 0));
        painter->setBrush(Qt::red);
        painter->drawPie(circle, startAngle*16.0, -fabs(redYellowAngleWarmup-startAngle)*16.0);
    } else {
        //Draw the upper red part
        painter->setPen(QPen(Qt::red, 0));
        painter->setBrush(Qt::red);
        painter->drawPie(circle, yellowRedAngle*16.0, -fabs(startAngle-spanAngle-yellowRedAngle)*16.0);

        //Draw the lower yellow part
        painter->setPen(QPen(Qt::yellow, 0));
        painter->setBrush(Qt::yellow);
        painter->drawPie(circle, redYellowAngle*16.0, -fabs(yellowGreenAngle-redYellowAngle)*16.0);

        //Draw the upper yellow part
        painter->setPen(QPen(Qt::yellow, 0));
        painter->setBrush(Qt::yellow);
        painter->drawPie(circle, greenYellowAngle*16.0, -fabs(yellowRedAngle-greenYellowAngle)*16.0);

        //Draw the lower red part
        painter->setPen(QPen(Qt::red, 0));
        painter->setBrush(Qt::red);
        painter->drawPie(circle, startAngle*16.0, -fabs(redYellowAngle-startAngle)*16.0);
    }


//    //Draw the upper red part
//    painter->setPen(QPen(Qt::red, 0));
//    painter->setBrush(Qt::red);
//    painter->drawPie(circle, greenRedAngle*16.0, -fabs(startAngle-spanAngle-greenRedAngle)*16.0);

	//Overlay the center with a black circle
	painter->setPen(QPen(Qt::black, 0));
	painter->setBrush(Qt::black);
	painter->drawEllipse(circle.center(), 110.0, 110.0);

	//Set the pen and font to draw the ticks
	painter->setPen(QPen(Qt::white, 2));
	painter->setFont(QFont("Arial", 14));
	foreach(double value, beetweenValues)
	{
		//Rotate painter and draw the ticks
		painter->save();
		painter->rotate(-calculateLocalValue(value));
		painter->drawLine(100, 0, 130, 0);
		painter->restore();
		//Define a box, move it and draw the text centered to this position
		QRectF textRect(-10, -10, 20, 20);
		textRect.moveCenter(QPointF(cos(calculateLocalValue(value)/180.0*M_PI)*85.0, -sin(calculateLocalValue(value)/180.0*M_PI)*85.0));
		painter->drawText(textRect, Qt::AlignCenter, QString::number(value/100.0, 'f', 0));
	}

	//Draw the center text
	QRectF centerTextRect(-50, -50, 100, 100);
	painter->drawText(centerTextRect, Qt::AlignCenter, "x 100 rpm");

	//Draw the needle if value is in range
	if((currentValue > minValue) &&
			(currentValue < maxValue))
	{
		//Needle is white with 1px black border
		painter->setPen(QPen(Qt::black, 1));
		painter->setBrush(Qt::white);
		//Define the shape
		QPolygonF marker;
		marker.append(QPointF(110.0, 0.0));
		marker.append(QPointF(140.0, -7.0));
		marker.append(QPointF(140.0, 7.0));
		//Rotate the painter and draw the needle
		painter->save();
		painter->rotate(-calculateLocalValue(currentValue));
		painter->drawPolygon(marker);
		painter->restore();
	}

	//If number is in red range, draw it red
    if ((currentValue > yellowRedBorder || currentValue < redYellowBorder) || (currentValue > yellowRedBorderWarmup || currentValue < redYellowBorderWarmup))
    {
        if (isWarmup && (currentValue > yellowRedBorderWarmup || currentValue < redYellowBorderWarmup))
        {

            painter->setPen(Qt::red);
            if (isAlarmed == false) {
                emit sendAlarm("RPM", Qt::red, false);
                isAlarmed = true;
            }

        } else if (isWarmup = false && (currentValue > yellowRedBorderWarmup || currentValue < redYellowBorderWarmup))
        {

            painter->setPen(Qt::red);
            if (isAlarmed == false) {
                emit sendAlarm("RPM", Qt::red, false);
                isAlarmed = true;
            }
        }
    }
    else
    {
        painter->setPen(Qt::white);

        if (isAlarmed == true) {
            emit cancelAlarm("RPM");
            isAlarmed = false;
        }

    }

	//Round value to the nearest 10
	QString rpm = QString::number(currentValue-fmod(currentValue, 10.0), 'f', 0);

	//Set position and font for the value and draw it
	QRectF textRect(-100, 35, 170, 65);
	painter->setFont(QFont("Arial", 30, 1));
	painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, rpm);
	//Set position and font for the unit and draw it
	QRectF unitRect(90, 35, 100, 65);
	painter->setFont(QFont("Arial", 20, 1));
    painter->drawText(unitRect, Qt::AlignLeft | Qt::AlignVCenter, "RPM");
}

void RpmIndicator::setStartSpan(double start, double span)
{
	startAngle = start;
	spanAngle = span;
}

void RpmIndicator::setBorders(double minimum, double maximum, double whiteGreen, double greenRed, double yellowRed, double greenYellow, double redYellow, double yellowGreen, double yellowRedWarmup, double greenYellowWarmup, double redYellowWarmup, double yellowGreenWarmup)
{
    minValue = minimum;
    maxValue = maximum;
    whiteGreenBorder = whiteGreen;
    greenRedBorder = greenRed;
    yellowRedBorder = yellowRed;
    greenYellowBorder = greenYellow;
    redYellowBorder = redYellow;
    yellowGreenBorder = yellowGreen;
    yellowRedBorderWarmup = yellowRedWarmup;
    greenYellowBorderWarmup = greenYellowWarmup;
    redYellowBorderWarmup = redYellowWarmup;
    yellowGreenBorderWarmup = yellowGreenWarmup;
}

double RpmIndicator::calculateLocalValue(double value) const
{
	return startAngle-((value-minValue)/(maxValue-minValue)*spanAngle);
}

void RpmIndicator::addBetweenValue(double value)
{
	beetweenValues.append(value);
}

void RpmIndicator::setValue(double value)
{
	currentValue = value;
	update();
}
