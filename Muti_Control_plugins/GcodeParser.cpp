#include "GcodeParser.h"
#include <cmath>

GcodeParser::GcodeParser(QObject *parent) : QObject(parent)
{
  m_Gcode_Q = new QQueue<Gcode_segment>();

  m_currentPoint = QVector3D(0,0,0);//初始化变量
  m_isMetric = true;
  m_inAbsoluteMode = true;
  m_inAbsoluteIJKMode = false;
  m_lastGcodeCommand = -1;
  m_commandNumber = 0;

  m_lastSpeed = 0;
  m_lastSpindleSpeed = 0;
  m_traverseSpeed = 300;

  m_isChangeRobot = false;

  this->init_machineList();

}

/**
* Add a command to be processed.
*/
int GcodeParser::addCommand(QString command)
{
    QString stripped = GcodePreprocessorUtils::removeComment(command);
    QStringList args = GcodePreprocessorUtils::splitCommand(stripped);
    return this->addCommand(args);
}

/**
* Add a command which has already been broken up into its arguments.
*/
int GcodeParser::addCommand(const QStringList &args)
{
    if (args.isEmpty()) {
        return NULL;
    }
    return processCommand(args);//处理命令
}

int GcodeParser::processCommand(const QStringList &args)
{
    QList<float> gCodes;
//    PointSegment *ps = NULL;

    int robotIndexTmp = 0;
    robotIndexTmp = (m_Gcode_Q->isEmpty()? 1:(m_Gcode_Q->last().index_robot));
    robotIndexTmp = robotIndexTmp==0? 1:robotIndexTmp;

    // Handle F code
    double speed = GcodePreprocessorUtils::parseCoord(args, 'F');
    if (!qIsNaN(speed)){
        this->m_lastSpeed = this->m_isMetric ? speed : speed * 25.4;
        this->m_List_Machine.at(robotIndexTmp)->speed_step = this->m_lastSpeed;
    }

    // Handle S code
    double spindleSpeed = GcodePreprocessorUtils::parseCoord(args, 'S');
    if (!qIsNaN(spindleSpeed)){
        this->m_lastSpindleSpeed = spindleSpeed;
        this->m_List_Machine.at(robotIndexTmp)->speed_spindle = this->m_lastSpindleSpeed;
    }

    // Handle P code
    double dwell = GcodePreprocessorUtils::parseCoord(args, 'P');
//    if (!qIsNaN(dwell)){
//        this->m_points.last()->setDwell(dwell);
//    }

    // handle G codes.
    gCodes = GcodePreprocessorUtils::parseCodes(args, 'G');

    // If there was no command, add the implicit one to the party.
//    if (gCodes.isEmpty() && m_lastGcodeCommand != -1) {//注释行
//        gCodes.append(m_lastGcodeCommand);
////        qDebug() << "ssd"<<xx++<<args;
//    }
  //NOTE:M代码必须要是新行才能识别
    if(gCodes.isEmpty()){//说明不是Gcode
        //解析Mcode
        QList<float> mCodes;
        mCodes = GcodePreprocessorUtils::parseCodes(args,'M');
        if(mCodes.isEmpty()){
            mCodes.append(Gcode_segment::Invalid_Mcode);//如果既不是Gcode也不是Mcode,则赋值为M-2表示无效代码
        }
        foreach (float code, mCodes) {
    //        ps = handleGCode(code, args);
            if(code == Gcode_segment::Invalid_Mcode){
                if (!qIsNaN(speed)) code = Gcode_segment::SpeedOnly_CODE;
                else if (!qIsNaN(spindleSpeed)) code = Gcode_segment::SpindleOnly_CODE;
                else if (!qIsNaN(dwell)) code = Gcode_segment::DwellOnly_CODE;
            }

            handleMCode(code);
//            qDebug() << code;
        }
//        qDebug() << "gCodes.isEmpty()" << args;
    }
    else{
        //m_Gcode_Q->clear();
        foreach (float code, gCodes) {
    //        ps = handleGCode(code, args);
            handleGCode(code, args);
        }
//        qDebug() << "gCodes.isEmpty() else" << args;
    }


//    return ps;
    return 0;
}

int GcodeParser::handleMCode(float code){
    int Mcode_temp = Gcode_segment::No_Mcode;
    if(code == 100.0f) Mcode_temp = Gcode_segment::MotorTool_On;
    else if(code == 101.0f)  Mcode_temp = Gcode_segment::MotorTool_Off;
    else if(code == 102.0f) Mcode_temp = Gcode_segment::MotorTool1_On;
    else if(code == 103.0f)  Mcode_temp = Gcode_segment::MotorTool1_Off;
    else if(code >= Gcode_segment::LIMIT_Wait_X && code <= Gcode_segment::LIMIT_Wait_C){
        Mcode_temp = (int)lround(code);
    }
    else if(code>=Gcode_segment::RobotChange && code <= (Gcode_segment::RobotChange+50)){
        Mcode_temp = (int)lround(code);
    }
    else if(code <= Gcode_segment::COMMENT_CODE){//如果是可以忽略的G代码,则也赋值即可
        Mcode_temp = (int)lround(code);
    }

//    else if(code == 500.0f)  Mcode_temp = Gcode_segment::RobotChange;
//    else if(code == 501.0f)  Mcode_temp = Gcode_segment::RobotChange1;
//    else if(code == 502.0f)  Mcode_temp = Gcode_segment::RobotChange2;
//    else if(code == 503.0f)  Mcode_temp = Gcode_segment::RobotChange3;

    int robotIndexTmp = 0;
    if(Mcode_temp > Gcode_segment::RobotChange){//如果是改变机器人编号
        this->m_isChangeRobot = true;
        robotIndexTmp = Mcode_temp - Gcode_segment::RobotChange;
    }
    else{
        robotIndexTmp = m_Gcode_Q->isEmpty()? 1:(m_Gcode_Q->last().index_robot);
    }

    robotIndexTmp = robotIndexTmp==0? 1:robotIndexTmp;//所有是robot0的,也是指robot1

    this->check_MachineList(robotIndexTmp);//检查下参数是否正确，并做修正

    Gcode_segment segment;
//    segment.line = m_Gcode_Q->size();
    segment.line = m_Gcode_Q->isEmpty()? 0:(m_Gcode_Q->last().line+1);
    segment.data_xyz = this->m_List_Machine.at(robotIndexTmp)->data_xyz;
    segment.Mcode = Mcode_temp;
    segment.speed_spindle = this->m_List_Machine.at(robotIndexTmp)->speed_spindle;
    segment.speed_step = this->m_List_Machine.at(robotIndexTmp)->speed_step;
    segment.index_robot = robotIndexTmp;
    m_Gcode_Q->enqueue(segment);
//    qDebug() << "handleMCode:"<<Mcode_temp;

    return 0;
}


int GcodeParser::handleGCode(float code, const QStringList &args)
{
//    PointSegment *ps = NULL;
    this->m_inAbsoluteMode = true;//设置为绝对坐标
    QVector3D nextPoint = GcodePreprocessorUtils::updatePointWithCommand(args, this->m_currentPoint, this->m_inAbsoluteMode);
//    qDebug() << qIsNaN(x)<< qIsNaN(y)<< qIsNaN(z);
//    qDebug() << nextPoint;

    int ret = 0;

    int robotIndexTmp = 0;
    robotIndexTmp = m_Gcode_Q->isEmpty()? 1:(m_Gcode_Q->last().index_robot);
    robotIndexTmp = robotIndexTmp==0? 1:robotIndexTmp;//所有是robot0的,也是指robot1
    this->check_MachineList(robotIndexTmp);//检查下参数是否正确，并做修正
    int currentLine = m_Gcode_Q->isEmpty()? 0:(m_Gcode_Q->last().line+1);

    if (code == 0.0f) ret = addLinearPointSegment(nextPoint, true);
    else if (code == 1.0f) ret = addLinearPointSegment(nextPoint, false);
    else if (code == 2.0f) ret = addLinearPointSegment(nextPoint, false);//暂时不支持圆弧插补
    else if (code == 3.0f) ret = addLinearPointSegment(nextPoint, false);//暂时不支持圆弧插补
    else if(code == 90.0f ){
        this->m_List_Machine.at(robotIndexTmp)->is_inAbsoluteMode = true;
        Gcode_segment segment;
        segment.line = currentLine;
        segment.data_xyz = QVector3D(0,0,0);
        segment.Mcode = Gcode_segment::G90_CODE;
        segment.speed_spindle = this->m_List_Machine.at(robotIndexTmp)->speed_spindle;
        segment.speed_step = this->m_List_Machine.at(robotIndexTmp)->speed_step;
        segment.index_robot = robotIndexTmp;
        m_Gcode_Q->enqueue(segment);

        this->m_currentPoint = QVector3D(0,0,0);//G92的时候会将当前的点置0
    }
    else if(code == 91.0f ){
        this->m_List_Machine.at(robotIndexTmp)->is_inAbsoluteMode = false;
        this->m_List_Machine.at(robotIndexTmp)->data_offsetBase = this->m_List_Machine.at(robotIndexTmp)->data_xyz;//设置当前坐标位置为基地址
        Gcode_segment segment;
        segment.line = currentLine;
        segment.data_xyz = QVector3D(0,0,0);
        segment.Mcode = Gcode_segment::G91_CODE;
        segment.speed_spindle = this->m_List_Machine.at(robotIndexTmp)->speed_spindle;
        segment.speed_step = this->m_List_Machine.at(robotIndexTmp)->speed_step;
        segment.index_robot = robotIndexTmp;
        m_Gcode_Q->enqueue(segment);

        this->m_currentPoint = QVector3D(0,0,0);//G92的时候会将当前的点置0
    }
    else if(code == 92.0f){
        this->m_List_Machine.at(robotIndexTmp)->is_inAbsoluteMode = false;
        this->m_List_Machine.at(robotIndexTmp)->data_offsetBase = nextPoint;//设置当前坐标位置为基地址
        Gcode_segment segment;
        segment.line = currentLine;
        segment.data_xyz = QVector3D(0,0,0);
        segment.Mcode = Gcode_segment::G92_CODE;
        segment.speed_spindle = this->m_List_Machine.at(robotIndexTmp)->speed_spindle;
        segment.speed_step = this->m_List_Machine.at(robotIndexTmp)->speed_step;
        segment.index_robot = robotIndexTmp;
        m_Gcode_Q->enqueue(segment);

        this->m_currentPoint = QVector3D(0,0,0);//G92的时候会将当前的点置0
    }
//    if (code == 0.0f) ps = addLinearPointSegment(nextPoint, true);
//    else if (code == 1.0f) ps = addLinearPointSegment(nextPoint, false);
//    else if (code == 38.2f) ps = addLinearPointSegment(nextPoint, false);
//    else if (code == 2.0f) ps = addArcPointSegment(nextPoint, true, args);
//    else if (code == 3.0f) ps = addArcPointSegment(nextPoint, false, args);
//    else if (code == 17.0f) this->m_currentPlane = PointSegment::XY;
//    else if (code == 18.0f) this->m_currentPlane = PointSegment::ZX;
//    else if (code == 19.0f) this->m_currentPlane = PointSegment::YZ;
//    else if (code == 20.0f) this->m_isMetric = false;
//    else if (code == 21.0f) this->m_isMetric = true;
//    else if (code == 90.0f) this->m_inAbsoluteMode = true;
//    else if (code == 90.1f) this->m_inAbsoluteIJKMode = true;
//    else if (code == 91.0f) this->m_inAbsoluteMode = false;
//    else if (code == 91.1f) this->m_inAbsoluteIJKMode = false;

//    if (code == 0.0f || code == 1.0f || code == 2.0f || code == 3.0f || code == 38.2f) this->m_lastGcodeCommand = code;

//    return ps;
    return ret;
}

int GcodeParser::addLinearPointSegment(const QVector3D &nextPoint, bool fastTraverse)
{
    QVector3D diffPoint = nextPoint - this->m_currentPoint;//获取两点之差
    float cartesian_mm = diffPoint.length();
    //NOTE:由于计算过程中存在计算误差，如果diff_mm设置的太小，会出现问题,而且PLC的周期如果太大，不易进行这种细分操作
    float diff_mm = 5;//设置为0.1mm
    long step = std::lrint(cartesian_mm/diff_mm);//lround是四舍五入，lrint是寻找大于float的数
    if(step<=0) step = 0;
    int currentLine = m_Gcode_Q->isEmpty()? 0:(m_Gcode_Q->last().line+1);
    float fraction;
//    qDebug() << "cartesian_mm"<<cartesian_mm<<currentLine;

    int robotIndexTmp = 0;
    robotIndexTmp = m_Gcode_Q->isEmpty()? 1:(m_Gcode_Q->last().index_robot);

    robotIndexTmp = robotIndexTmp==0? 1:robotIndexTmp;//所有是robot0的,也是指robot1

    this->check_MachineList(robotIndexTmp);//检查下参数是否正确，并做修正

    Gcode_segment segment;
    segment.line = currentLine;
    if(this->m_List_Machine.at(robotIndexTmp)->is_inAbsoluteMode){
        segment.data_xyz = nextPoint;
    }
    else{
        segment.data_xyz = this->m_List_Machine.at(robotIndexTmp)->data_offsetBase + nextPoint;
    }
    segment.Mcode = Gcode_segment::No_Mcode;
    segment.speed_spindle = this->m_List_Machine.at(robotIndexTmp)->speed_spindle;
    segment.speed_step = this->m_List_Machine.at(robotIndexTmp)->speed_step;
    segment.index_robot = robotIndexTmp;

    this->m_List_Machine.at(robotIndexTmp)->data_xyz  = segment.data_xyz;

    //WARNING:这里的细分操作总是会有误差
//    if(cartesian_mm<20 && this->m_isChangeRobot==false){ //防止距离太大，队列长度太长
//        for(long i=1;i<step;i++){
//           fraction = (float)i/(float)step;
//           segment.data_xyz.setX(this->m_currentPoint.x() + diffPoint.x()*fraction);
//           segment.data_xyz.setY(this->m_currentPoint.y() + diffPoint.y()*fraction);
//           segment.data_xyz.setZ(this->m_currentPoint.z() + diffPoint.z()*fraction);

//           m_Gcode_Q->enqueue(segment);
//        }
//    }

    this->m_isChangeRobot = false;


    //将最后一段加入队列中
//    segment.line = m_Gcode_Q->size();
//    segment.line = m_Gcode_Q->isEmpty()? 0:(m_Gcode_Q->last().line+1);
//    segment.data_xyz = nextPoint;
//    segment.Mcode = Gcode_segment::No_Mcode;
//    segment.speed_spindle = this->m_lastSpindleSpeed;
//    segment.speed_step = this->m_lastSpeed;
//    segment.data_xyz = nextPoint;
    m_Gcode_Q->enqueue(segment);

    this->m_currentPoint = nextPoint;


//    PointSegment *ps = new PointSegment(&nextPoint, m_commandNumber++);

//    bool zOnly = false;

//    // Check for z-only
//    if ((this->m_currentPoint.x() == nextPoint.x()) &&
//            (this->m_currentPoint.y() == nextPoint.y()) &&
//            (this->m_currentPoint.z() != nextPoint.z())) {
//        zOnly = true;
//    }

//    ps->setIsMetric(this->m_isMetric);
//    ps->setIsZMovement(zOnly);
//    ps->setIsFastTraverse(fastTraverse);
//    ps->setIsAbsolute(this->m_inAbsoluteMode);
//    ps->setSpeed(fastTraverse ? this->m_traverseSpeed : this->m_lastSpeed);
//    ps->setSpindleSpeed(this->m_lastSpindleSpeed);
//    this->m_points.append(ps);

//    // Save off the endpoint.
//    this->m_currentPoint = nextPoint;

//    return ps;

    return 0;
}

QQueue<Gcode_segment>* GcodeParser::getGodeQueue(){
    return m_Gcode_Q;
}

void GcodeParser::clearQueue(){
    m_Gcode_Q->clear();
}

void GcodeParser::setCurrentPoint(const QVector3D point)
{
    m_currentPoint = point;
}

void GcodeParser::init_machineList()
{
    m_List_Machine.clear();
    //每次添加2个元素,第一个元素不用
    Gcode_segment *segment = nullptr;

    segment = new Gcode_segment();//保证一定有一台机器
    m_List_Machine.append(segment);
    segment = new Gcode_segment();//保证一定有一台机器
    m_List_Machine.append(segment);
}

///
/// \brief 检查MachineList是否需要添加新的元素(第一个没有用到)
/// \param robotIndex
///
void GcodeParser::check_MachineList(int robotIndex)
{
    Gcode_segment *segment = nullptr;

    if(robotIndex >= m_List_Machine.size()){
        int loop_num = robotIndex - m_List_Machine.size() + 1;
        while(loop_num -- > 0){
            segment = new Gcode_segment();
            m_List_Machine.append(segment);
        }
    }
}
