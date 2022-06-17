#include "myrobot.h"
#include <cmath>
#include <math.h>

MyRobot::MyRobot(QObject *parent) : QObject(parent) {

    m_etat = 0;
    m_manette = false;
    DataToSend.resize(9);
    //Pour Ã©viter le CRC (ne pas changer)
    DataToSend[0] = 0xFF;
    //Taille (ne pas changer)
    DataToSend[1] = 0x07;
    //Vitesse de la gauche
    DataToSend[2] = 0x0;
    DataToSend[3] = 0x0;
    //Vitesse de la droite
    DataToSend[4] = 0x0;
    DataToSend[5] = 0x0;
    //A determiner
    DataToSend[6] = 0x0;
    //CRC (ne pas changer)
    DataToSend[7] = 0x0;
    DataToSend[8] = 0x0;
    DataReceived.resize(21);
    TimerEnvoi = new QTimer();
    //Initialisation de la vitesse
    m_vitesse = 120;
    // setup signal and slot
    connect(TimerEnvoi, SIGNAL(timeout()), this, SLOT(MyTimerSlot())); //Send data to wifibot timer
}


void MyRobot::doConnect() {
    socket = new QTcpSocket(this); // socket creation
    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
    qDebug() << "connecting..."; // this is not blocking call
    //socket->connectToHost("LOCALHOST", 15020);
    socket->connectToHost("192.168.1.106", 15020); // connection to wifibot
    // we need to wait...
    if(!socket->waitForConnected(5000)) {
        qDebug() << "Error: " << socket->errorString();
        return;
    }
    TimerEnvoi->start(75);

}

void MyRobot::disConnect() {
    TimerEnvoi->stop();
    socket->close();
}

void MyRobot::connected() {
    qDebug() << "connected..."; // Hey server, tell me about you.
}

void MyRobot::disconnected() {
    qDebug() << "disconnected...";
}

void MyRobot::bytesWritten(qint64 bytes) {
    qDebug() << bytes << " bytes written...";
}

void MyRobot::readyRead() {
    qDebug() << "reading..."; // read the data from the socket
    DataReceived = socket->readAll();
    emit updateUI(DataReceived);
    qDebug() << DataReceived[0] << DataReceived[1] << DataReceived[2];
}

void MyRobot::MyTimerSlot() {
    qDebug() << "Timer...";
    if(m_manette == false){
        move(m_etat);
    }
    else{
        move_xbox();
    }

    while(Mutex.tryLock());
    socket->write(DataToSend);
    DataReceived = socket->readAll();
    Mutex.unlock();
}

void MyRobot::move(int cas){

    //Pour Ã©viter le CRC (ne pas changer)
    DataToSend[0] = 0xFF;
    //Taille (ne pas changer)
    DataToSend[1] = 0x07;
    //Vitesse de la gauche
    DataToSend[2] = 0x0;
    DataToSend[3] = 0x0;
    //Vitesse de la droite
    DataToSend[4] = 0x0;
    DataToSend[5] = 0x0;
    //Avancer ou reculer
    DataToSend[6] = 0x0;

       Mutex.tryLock();
    switch(cas) {
    case 1 :    //Avancer
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;   //F0 pour 240 (on sait jamais)  78 pour 120     B4 pour 180
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        //Avancer ou reculer
        DataToSend[6] = 0x50;
        break;
    case 2 :    //gauche
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        //Avancer ou reculer
        DataToSend[6] = 0x10;   //Aller que Ã  doite = 16
        break;
    case 3 :    //droite
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        //Avancer ou reculer
        DataToSend[6] = 0x40;   //Aller que Ã  gauche = 64
        break;
    case 4 :    //reculer
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        break;
    case 5 :    //STOP
        break;
    case 6 :    //haut gauche
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        //Avancer ou reculer
        DataToSend[6] = 0x50;
        break;
    case 7 :    //bas gauche
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        break;
    case 8 :    //haut droite
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;
        //Avancer ou reculer
        DataToSend[6] = 0x50;
        break;
    case 9 :    //bas droite
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;
        break;
    case 10 :   //test de manette
        //Vitesse de la gauche
        DataToSend[2] = m_vitesse;   //F0 pour 240 (on sait jamais)  78 pour 120     B4 pour 180
        //Vitesse de la droite
        DataToSend[4] = m_vitesse;
        //Avancer ou reculer
        DataToSend[6] = 0x50;
        break;
    }
    DataToSend[7] = 0x0;
    qint64 c16 = Crc16(DataToSend, 1);  //7 octets hors CRC

    //CRC (ne pas changer)
    DataToSend[7] = (unsigned char)c16;    //Partie basse du CRC
    DataToSend[8] = (unsigned char)(c16 >> 8);   //Partie haute


    Mutex.unlock();
}

void MyRobot::move_xbox(){

    //Pour Ã©viter le CRC (ne pas changer)
    DataToSend[0] = 0xFF;
    //Taille (ne pas changer)
    DataToSend[1] = 0x07;
    //Vitesse de la gauche
    DataToSend[2] = 0x0;
    DataToSend[3] = 0x0;
    //Vitesse de la droite
    DataToSend[4] = 0x0;
    DataToSend[5] = 0x0;
    //Avancer ou reculer
    DataToSend[6] = 0x0;

    Mutex.tryLock();

    //On parcourt le cercle trigonomÃ©trique dans le sens des aiguilles d'une montre
    if((m_xbox_x > 0) && (m_xbox_y < 0)){
        if(-(m_xbox_y) > m_xbox_x){   //Cas 1
            DataToSend[2] = m_vitesse;
            DataToSend[4] = m_vitesse - ((2*m_xbox_x/sqrt(2))*m_vitesse);
            DataToSend[6] = 0x50;   //Avancer gauche et droite
        }
        else {  //Cas 2
            DataToSend[2] = m_vitesse;
            DataToSend[4] = m_vitesse - ((2*m_xbox_y/sqrt(2))*m_vitesse);
            DataToSend[6] = 0x40;   //Avancer gauche
        }
    }
    else if((m_xbox_x > 0) && (m_xbox_y > 0)){
        if(m_xbox_y < m_xbox_x){   //Cas 3
            DataToSend[2] = m_vitesse;
            DataToSend[4] = m_vitesse - ((-2*m_xbox_y/sqrt(2))*m_vitesse);
            DataToSend[6] = 0x10;   //Reculer gauche : Pour reculer vers la droite on doit faire avancer la roue de droite et faire reculer la roue de gauche
        }
        else {  //Cas 4
            DataToSend[2] = m_vitesse;
            DataToSend[4] = m_vitesse - ((2*m_xbox_x/sqrt(2))*m_vitesse);
            DataToSend[6] = 0x0;    //Reculer
        }
    }
    else if((m_xbox_x < 0) && (m_xbox_y > 0)){
        if(m_xbox_y > -(m_xbox_x)){   //Cas 5
            DataToSend[2] = m_vitesse - ((-2*m_xbox_x/sqrt(2))*m_vitesse);
            DataToSend[4] = m_vitesse;
            DataToSend[6] = 0x0;   //Reculer
        }
        else {  //Cas 6
            DataToSend[2] = m_vitesse - ((-2*m_xbox_y/sqrt(2))*m_vitesse);
            DataToSend[4] = m_vitesse;
            DataToSend[6] = 0x40;    //Reculer droite : Pour reculer vers la gauche on doit faire avancer la roue de gauche et faire reculer la roue de droite
        }
    }
    else if((m_xbox_x < 0) && (m_xbox_y < 0)){
        if(-(m_xbox_y) < -(m_xbox_x)){   //Cas 7
            DataToSend[2] = m_vitesse - ((2*m_xbox_y/sqrt(2))*m_vitesse);
            DataToSend[4] = m_vitesse;
            DataToSend[6] = 0x10;   //Avancer droite
        }
        else {  //Cas 8
            DataToSend[2] = m_vitesse - ((-2*m_xbox_x/sqrt(2))*m_vitesse);
            DataToSend[4] = m_vitesse;
            DataToSend[6] = 0x50;   //Avancer gauche et droite
        }
    }


    DataToSend[7] = 0x0;
    qint64 c16 = Crc16(DataToSend, 1);  //7 octets hors CRC

    //CRC (ne pas changer)
    DataToSend[7] = (unsigned char)c16;    //Partie basse du CRC
    DataToSend[8] = (unsigned char)(c16 >> 8);   //Partie haute


    Mutex.unlock();
}

qint64 MyRobot::Crc16(QByteArray Adresse_tab , int pos)
{
Adresse_tab.data();
unsigned char *data = (unsigned char*)Adresse_tab.constData();
qint64 Crc = 0xFFFF;
qint64 Polynome = 0xA001;
qint64 Parity= 0;
for (; pos < Adresse_tab.length() - 2; pos++) //7 octets
    {
    Crc ^= *(data+pos);
    for ( unsigned int CptBit = 0; CptBit <= 7 ; CptBit++)  //8 bits (par octets)
        {
        Parity= Crc;
        Crc >>= 1;
        if (Parity%2 == true) Crc ^= Polynome;
        }
    }
return Crc;
}

void MyRobot::set_vitesse(int valeur){
    m_vitesse = valeur;
}

int MyRobot::get_vitesse(){
    return m_vitesse;
}

void MyRobot::set_etat(int valeur){
    m_etat = valeur;
}
int MyRobot::get_etat(){
    return m_etat;
}
void MyRobot::set_manette(bool valeur){
    m_manette = valeur;
}

void MyRobot::set_xbox_x(double valeur){
    m_xbox_x = valeur;
}

void MyRobot::set_xbox_y(double valeur){
    m_xbox_y = valeur;
}

int MyRobot::get_IRG1() { // get the first left IR sensor value
    unsigned char IR = DataReceived[3];
    return IR;
};
int MyRobot::get_IRG2() { // get the second left IR sensor value
    unsigned char IR = DataReceived[12];
    return IR;
};
int MyRobot::get_IRD1() { // get the first right IR sensor value
    unsigned char IR = DataReceived[11];
    return IR;
};
int MyRobot::get_IRD2() { // get the second right IR sensor value
    unsigned char IR = DataReceived[4];
    return IR;
};



