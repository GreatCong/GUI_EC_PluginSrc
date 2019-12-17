#include "GcodePreprocessorUtils.h"

/**
* Removes any comments within parentheses or beginning with a semi-colon.
*/
QString GcodePreprocessorUtils::removeComment(QString command)
{
    static QRegExp rx1("\\(+[^\\(]*\\)+");
    static QRegExp rx2(";.*");

    // Remove any comments within ( parentheses ) using regex "\([^\(]*\)"
    if (command.contains('(')) command.remove(rx1);

    // Remove any comment beginning with ';' using regex ";.*"
    if (command.contains(';')) command.remove(rx2);

    return command.trimmed();
}

///**
//* Splits a gcode command by each word/argument, doesn't care about spaces.
//* This command is about the same speed as the string.split(" ") command,
//* but might be a little faster using precompiled regex.
//*/
QStringList GcodePreprocessorUtils::splitCommand(const QString &command) {
    QStringList l;
    bool readNumeric = false;
    QString sb;

    QByteArray ba(command.toLatin1());
    const char *cmd = ba.constData(); // Direct access to string data
    char c;

    for (int i = 0; i < command.length(); i++) {
        c = cmd[i];

        if (readNumeric && !isDigit(c) && c != '.') {
            readNumeric = false;
            l.append(sb);
            sb.clear();
            if (isLetter(c)) sb.append(c);
        } else if (isDigit(c) || c == '.' || c == '-') {
            sb.append(c);
            readNumeric = true;
        } else if (isLetter(c)) sb.append(c);
    }

    if (sb.length() > 0) l.append(sb);

//    QChar c;

//    for (int i = 0; i < command.length(); i++) {
//        c = command[i];

//        if (readNumeric && !c.isDigit() && c != '.') {
//            readNumeric = false;
//            l.append(sb);
//            sb = "";
//            if (c.isLetter()) sb.append(c);
//        } else if (c.isDigit() || c == '.' || c == '-') {
//            sb.append(c);
//            readNumeric = true;
//        } else if (c.isLetter()) sb.append(c);
//    }

//    if (sb.length() > 0) l.append(sb);

    return l;
}

/**
* Searches for a comment in the input string and returns the first match.
*/
QString GcodePreprocessorUtils::parseComment(QString command)
{
    // REGEX: Find any comment, includes the comment characters:
    // "(?<=\()[^\(\)]*|(?<=\;)[^;]*"
    // "(?<=\\()[^\\(\\)]*|(?<=\\;)[^;]*"

    static QRegExp re("(\\([^\\(\\)]*\\)|;[^;].*)");

    if (re.indexIn(command) != -1) {
        return re.cap(1);
    }
    return "";
}

QString GcodePreprocessorUtils::truncateDecimals(int length, QString command)
{
    static QRegExp re("(\\d*\\.\\d*)");
    int pos = 0;

    while ((pos = re.indexIn(command, pos)) != -1)
    {
        QString newNum = QString::number(re.cap(1).toDouble(), 'f', length);
        command = command.left(pos) + newNum + command.mid(pos + re.matchedLength());
        pos += newNum.length() + 1;
    }

    return command;
}

QString GcodePreprocessorUtils::removeAllWhitespace(QString command)
{
    static QRegExp rx("\\s");

    return command.remove(rx);
}

QList<float> GcodePreprocessorUtils::parseCodes(const QStringList &args, char code)
{
    QList<float> l;

    foreach (QString s, args) {
        if (s.length() > 0 && s[0].toUpper() == code) l.append(s.mid(1).toDouble());
    }

    return l;
}

// TODO: Replace everything that uses this with a loop that loops through
// the string and creates a hash with all the values.
double GcodePreprocessorUtils::parseCoord(QStringList argList, char c)
{
//    int n = argList.length();

//    for (int i = 0; i < n; i++) {
//        if (argList[i].length() > 0 && argList[i][0].toUpper() == c) return argList[i].mid(1).toDouble();
//    }

    foreach (QString t, argList)
    {
        if (t.length() > 0 && t[0].toUpper() == c) return t.mid(1).toDouble();
    }
    return qQNaN();
}

QList<int> GcodePreprocessorUtils::parseGCodes(QString command)
{
    static QRegExp re("[Gg]0*(\\d+)");

    QList<int> codes;
    int pos = 0;

    while ((pos = re.indexIn(command, pos)) != -1) {
        codes.append(re.cap(1).toInt());
        pos += re.matchedLength();
    }

    return codes;
}

QList<int> GcodePreprocessorUtils::parseMCodes(QString command)
{
    static QRegExp re("[Mm]0*(\\d+)");

    QList<int> codes;
    int pos = 0;

    while ((pos = re.indexIn(command, pos)) != -1) {
        codes.append(re.cap(1).toInt());
        pos += re.matchedLength();
    }

    return codes;
}

/**
* Update a point given the arguments of a command.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(const QString &command, const QVector3D &initial, bool absoluteMode)
{
    QStringList l = splitCommand(command);
    return updatePointWithCommand(l, initial, absoluteMode);
}

/**
* Update a point given the arguments of a command, using a pre-parsed list.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(const QStringList &commandArgs, const QVector3D &initial,
                                                         bool absoluteMode)
{
    double x = qQNaN();
    double y = qQNaN();
    double z = qQNaN();
    char c;

    for (int i = 0; i < commandArgs.length(); i++) {
        if (commandArgs.at(i).length() > 0) {
            c = commandArgs.at(i).at(0).toUpper().toLatin1();
            switch (c) {
            case 'X':
                x = commandArgs.at(i).mid(1).toDouble();;
                break;
            case 'Y':
                y = commandArgs.at(i).mid(1).toDouble();;
                break;
            case 'Z':
                z = commandArgs.at(i).mid(1).toDouble();;
                break;
            }
        }
    }
//    qDebug() << qIsNaN(x)<< qIsNaN(y)<< qIsNaN(z);
    return updatePointWithCommand(initial, x, y, z, absoluteMode);
}

/**
* Update a point given the new coordinates.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode)
{
    QVector3D newPoint(initial.x(), initial.y(), initial.z());

    if (absoluteMode) {
        if (!qIsNaN(x)) newPoint.setX(x);
        if (!qIsNaN(y)) newPoint.setY(y);
        if (!qIsNaN(z)) newPoint.setZ(z);
    } else {
        if (!qIsNaN(x)) newPoint.setX(newPoint.x() + x);
        if (!qIsNaN(y)) newPoint.setY(newPoint.y() + y);
        if (!qIsNaN(z)) newPoint.setZ(newPoint.z() + z);
    }

    return newPoint;
}

QVector3D GcodePreprocessorUtils::updateCenterWithCommand(QStringList commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise)
{
    double i = qQNaN();
    double j = qQNaN();
    double k = qQNaN();
    double r = qQNaN();
    char c;

    foreach (QString t, commandArgs)
    {
        if (t.length() > 0) {
            c = t[0].toUpper().toLatin1();
            switch (c) {
            case 'I':
                i = t.mid(1).toDouble();
                break;
            case 'J':
                j = t.mid(1).toDouble();
                break;
            case 'K':
                k = t.mid(1).toDouble();
                break;
            case 'R':
                r = t.mid(1).toDouble();
                break;
            }
        }
    }

    if (qIsNaN(i) && qIsNaN(j) && qIsNaN(k)) {
        return convertRToCenter(initial, nextPoint, r, absoluteIJKMode, clockwise);
    }

    return updatePointWithCommand(initial, i, j, k, absoluteIJKMode);
}

QVector3D GcodePreprocessorUtils::convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise) {
    double R = radius;
    QVector3D center;

    double x = end.x() - start.x();
    double y = end.y() - start.y();

    double h_x2_div_d = 4 * R * R - x * x - y * y;
    if (h_x2_div_d < 0) { qDebug() << "Error computing arc radius."; }
    h_x2_div_d = (-sqrt(h_x2_div_d)) / hypot(x, y);

    if (!clockwise) h_x2_div_d = -h_x2_div_d;

    // Special message from gcoder to software for which radius
    // should be used.
    if (R < 0) {
        h_x2_div_d = -h_x2_div_d;
        // TODO: Places that use this need to run ABS on radius.
        radius = -radius;
    }

    double offsetX = 0.5 * (x - (y * h_x2_div_d));
    double offsetY = 0.5 * (y + (x * h_x2_div_d));

    if (!absoluteIJK) {
        center.setX(start.x() + offsetX);
        center.setY(start.y() + offsetY);
    } else {
        center.setX(offsetX);
        center.setY(offsetY);
    }

    return center;
}

bool GcodePreprocessorUtils::isDigit(char c)
{
    return c > 47 && c < 58;
}

bool GcodePreprocessorUtils::isLetter(char c)
{
    return (c > 64 && c < 91) || (c > 96 && c < 123);
}

char GcodePreprocessorUtils::toUpper(char c)
{
    return (c > 96 && c < 123) ? c - 32 : c;
}
