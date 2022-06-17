#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUrl>
#include <QWebEngineView>

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{

    ui->setupUi(this);

    robot = new MyRobot(this);


          TimerReceiveIR = new QTimer(this);

           //Recupere la valeur de la batterie et l'affiche sur l'interface
          connect(robot, SIGNAL(updateUI(QByteArray)), this, SLOT(batterie()));
          connect(robot, SIGNAL(updateUI(QByteArray)), this, SLOT(collision()));

           //Recupere la valeur des infrarouges et l'affiche sur l'interface
          connect(TimerReceiveIR, &QTimer::timeout, this, &MainWindow::displayIRG1);
          connect(TimerReceiveIR, &QTimer::timeout, this, &MainWindow::displayIRG2);
          connect(TimerReceiveIR, &QTimer::timeout, this, &MainWindow::displayIRD1);
          connect(TimerReceiveIR, &QTimer::timeout, this, &MainWindow::displayIRD2);
          TimerReceiveIR->start(100);

          //Création de la manette et de ses actions
         xbox = new QGamepad();
         QObject::connect(xbox, SIGNAL(axisLeftXChanged(double)), this, SLOT(move_xbox()));
         QObject::connect(xbox, SIGNAL(axisLeftYChanged(double)), this, SLOT(move_xbox()));
         QObject::connect(xbox, SIGNAL(buttonBChanged(bool)), this, SLOT(stop()));
         QObject::connect(xbox, SIGNAL(axisRightXChanged(double)), this, SLOT(move_xbox()));
         QObject::connect(xbox, SIGNAL(axisRightYChanged(double)), this, SLOT(move_xbox()));


           //Recuperer la valeur de l'a batterie'odometrie et l'affiche sur l'interface
          connect(robot, SIGNAL(updateUI(QByteArray)), this, SLOT(odometrieD()));
          connect(robot, SIGNAL(updateUI(QByteArray)), this, SLOT(odometrieG()));
          manager = new QNetworkAccessManager();

            //Vue de la webcam du robot (indÃ©pendant du robot)
          view = new QWebEngineView();
          view->load(QUrl("http://192.168.1.106:8080/?action=stream"));
          view->show();

       this->ui->camera->addWidget(view);


}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
}

//UTILISATION DU CLAVIER

//Lorsque l'on appuie sur une touche, Ã§a va affectuer une des fonctions
void MainWindow::keyPressEvent(QKeyEvent* key_robot){
    switch(key_robot->key()){
    case Qt::Key_Z :    //Avancer
        robot->set_etat(1);
        break;
    case Qt::Key_Q :    //Gauche
        robot->set_etat(2);
        break;
    case Qt::Key_D :    //Droite
        robot->set_etat(3);
        break;
    case Qt::Key_S :    //Reculer
        robot->set_etat(4);
        break;
    case Qt::Key_R :    //Stop
        robot->set_etat(5);
        break;
    case Qt::Key_W :    //Filtre Gris
        cam_filtre(1);
        break;
    case Qt::Key_X :    //Filtre Inverseur
        cam_filtre(2);
        break;
    case Qt::Key_C :    //Filtre sepia
        cam_filtre(3);
        break;
    case Qt::Key_V :    //Filtre specificateur de flou Gaussien
        cam_filtre(4);
        break;
    case Qt::Key_B :    //Filtre doubles saturation
        cam_filtre(5);
        break;
    case Qt::Key_J :    //Filtre ajusteur de luminosité
        cam_filtre(6);
        break;
    case Qt::Key_N :
        cam_reset();
        break;
    }
}

//Lorsque l'on relache, on s'arrete
void MainWindow::keyReleaseEvent(QKeyEvent* key_robot){
    robot->set_etat(5);
}

//Fonctions reliÃ©es aux boutons :
void MainWindow::connexion()
{
   robot->doConnect();
}
void MainWindow::deconnexion()
{
   robot->disConnect();
}
void MainWindow::avancer()
{
    robot->set_etat(1);

}
void MainWindow::gauche()
{
     robot->set_etat(2);
}
void MainWindow::droite()
{
     robot->set_etat(3);
}
void MainWindow::reculer()
{
     robot->set_etat(4);
}
void MainWindow::stop()
{
     robot->set_etat(5);
}
void MainWindow::hgauche()
{
     robot->set_etat(6);
}
void MainWindow::bgauche()
{
     robot->set_etat(7);
}
void MainWindow::hdroite()
{
     robot->set_etat(8);
}
void MainWindow::bdroite()
{
     robot->set_etat(9);
}


//MOUVEMENT INTERFACE
void MainWindow::on_Gauche_pressed()
{
    gauche();
}
void MainWindow::on_avancer_pressed()
{
    avancer();
}
void MainWindow::on_droite_pressed()
{
    droite();
}
void MainWindow::on_reculer_pressed()
{
    reculer();
}
void MainWindow::on_hgauche_pressed()
{
    hgauche();
}
void MainWindow::on_hdroite_pressed()
{
    hdroite();
}
void MainWindow::on_bgauche_pressed()
{
    bgauche();
}
void MainWindow::on_bdroite_pressed()
{
    bdroite();
}
void MainWindow::on_connexion_clicked()
{
    connexion();
}
void MainWindow::on_deconnexion_clicked()
{
    deconnexion();
}
void MainWindow::on_avancer_released()
{
    stop();
}
void MainWindow::on_hdroite_released()
{
    stop();
}
void MainWindow::on_droite_released()
{
    stop();
}
void MainWindow::on_bdroite_released()
{
    stop();
}
void MainWindow::on_bgauche_released()
{
    stop();
}
void MainWindow::on_Gauche_released()
{
    stop();
}
void MainWindow::on_hgauche_released()
{
    stop();
}
void MainWindow::on_reculer_released()
{
    stop();
}


//CHANGEMENT VITESSE ET AFFICHAGE
void MainWindow::on_slide_vitesse_valueChanged(int value)
{
    //Change la vitesse du robot
    robot->set_vitesse(value);
    //Affiche la vitesse et le pourcentage sur l'interface
    int vit = robot->get_vitesse();
    QString vit_string = QString::number(vit);
    double pourcentage = ((vit-120)*100/120);
    QString pourcentage_string = QString::number(pourcentage);
    ui->titre_vitesse->setText(vit_string + " (" + pourcentage_string + "%)");
}

//MOUVEMENT CAMERA
void MainWindow::on_haut_camera_pressed()
{
    cam_haut();
}
void MainWindow::on_gauche_camera_pressed()
{
    cam_gauche();
}
void MainWindow::on_droite_camera_pressed()
{
    cam_droite();
}
void MainWindow::on_bas_camera_pressed()
{
    cam_bas();
}
void MainWindow::cam_haut()
{
    request.setUrl(QUrl("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094853&group=1&value=-200"));
    manager->get(request);
}
void MainWindow::cam_bas()
{
    request.setUrl(QUrl("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094853&group=1&value=200"));
    manager->get(request);

}
void MainWindow::cam_gauche()
{
    request.setUrl(QUrl("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094852&group=1&value=200"));
    manager->get(request);
}
void MainWindow::cam_droite()
{
    request.setUrl(QUrl("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094852&group=1&value=-200"));
    manager->get(request);
}

//AFFICHAGE BATTERIE
void MainWindow::batterie(){

    unsigned char batterie = (robot->DataReceived[2]);
    int valeur = (int)batterie;
    valeur = (valeur-100) * 5;
    QString bat_string = QString::number(valeur);

    if(((unsigned int) valeur <= 100) && ((unsigned int) valeur > 20)){
        ui->barre_batterie->setValue(valeur); //Si marche pas : test avec (int)robot->DataReceived[2]
        ui->titre_batterie->setText("Batterie : " + bat_string + "%");
    }
    else if(((unsigned int) valeur > 100)){
        ui->barre_batterie->setValue(100);
        ui->titre_batterie->setText("Batterie : En charge");
    }
    else{
        ui->barre_batterie->setValue(0);
        ui->titre_batterie->setText("Plus de batterie");
    }
}

//INFRAROUGE
void MainWindow::displayIRG1(){

    int ir = robot->get_IRG1();
    ui->ir_AvG->display(ir);
}
void MainWindow::displayIRG2(){

    int ir = robot->get_IRG2();
    ui->ir_ArG->display(ir);
}
void MainWindow::displayIRD1(){

    int ir = robot->get_IRD1();
    ui->ir_AvD->display(ir);
}
void MainWindow::displayIRD2(){

    int ir = robot->get_IRD2();
    ui->ir_ArD->display(ir);
}

//ODOMETRIE
void MainWindow::odometrieD(){
    long odo =long((((long)robot->DataReceived[16] << 24))+(((long)robot->DataReceived[15] << 16)) +(((long)robot->DataReceived[14] << 8)) +((long)robot->DataReceived[13]));
    odo = (unsigned int) odo/2448;
    ui->odo_droit->display((int)odo);
    qDebug()<<"odometrieD";

}
void MainWindow::odometrieG(){
    long odo =long((((long)robot->DataReceived[8] << 24))+(((long)robot->DataReceived[7] << 16)) +(((long)robot->DataReceived[6] << 8)) +((long)robot->DataReceived[5]));
    odo = (unsigned int) odo/2448;
    ui->odo_gauche->display((int)odo);
     qDebug()<<"odometrieG";
}

//MANETTE
void MainWindow::move_xbox(){


    if((xbox->axisLeftX() == 0) && (xbox->axisLeftY() ==0)){
        robot->set_manette(false);
    }
    else{
        robot->set_xbox_x(xbox->axisLeftX());
        robot->set_xbox_y(xbox->axisLeftY());
        robot->set_manette(true);
    }

}
void MainWindow::direction_cam_manette()
{
    //On parcourt le cercle trigonométrique dans le sens des aiguilles d'une montre
    if((xbox->axisLeftX() < 0.5) && (xbox->axisLeftX() > -0.5)){    //A faire
        if(-(m_xbox_y) > m_xbox_x){   //Cas 1

        }
        else {  //Cas 2

        }
    }
    else if((m_xbox_x > 0) && (m_xbox_y > 0)){
        if(m_xbox_y < m_xbox_x){   //Cas 3

        }
        else {  //Cas 4

        }
    }
    else if((m_xbox_x < 0) && (m_xbox_y > 0)){
        if(m_xbox_y > -(m_xbox_x)){   //Cas 5

        }
        else {  //Cas 6

        }
    }
    else if((m_xbox_x < 0) && (m_xbox_y < 0)){
        if(-(m_xbox_y) < -(m_xbox_x)){   //Cas 7

        }
        else {  //Cas 8

        }
    }
}

//DETECTION ET ANNONCE D'OBSTACLE
/*void MainWindow::collision(){
    //On recupere les valeurs des infrarouges et on affiche si une valeur est supérieure à 100
    unsigned char  irAvG= robot->DataReceived[3];
    QString valeur_avg = QString::number(irAvG);
    int val_avg = valeur_avg.toInt();
    unsigned char irAvD = robot->DataReceived[11];
    QString valeur_avd = QString::number(irAvD);
    int val_avd = valeur_avd.toInt();
    unsigned char irArG = robot->DataReceived[12];
    QString valeur_arg = QString::number(irArG);
    int val_arg = valeur_arg.toInt();
    unsigned char irArD = robot->DataReceived[4];
    QString valeur_ard = QString::number(irArD);
    int val_ard = valeur_ard.toInt();
    if ((val_avg > 100) && (val_avg > val_avd) && (val_avg > val_ard) && (val_avg > val_arg))
    {
        ui->collision->setText("Attention, obstacle detecté devant à gauche");
        hdroite();
    }
    else if ((val_avd > 100) && (val_avd > val_avg) && (val_avd > val_ard) && (val_avg > val_arg))
    {
        ui->collision->setText("Attention, obstacle detecté devant à droite");
        hgauche();
    }
    else if ((robot->get_etat() == 4) && (val_ard > 100) && (val_avd > val_arg) && (val_ard > val_avg) && (val_ard > val_avd))
    {
        ui->collision->setText("Attention, obstacle detecté derriere à droite");
        bgauche();
    }
    else if ((robot->get_etat() == 4) && (val_arg > 100) && (val_avg > val_ard) && (val_arg > val_avg) && (val_arg > val_avd))
    {
        ui->collision->setText("Attention, obstacle detecté derriere à gauche");
        bdroite();
    }
    else
    {
        ui->collision->setText("aucun obstacle detecté");
    }

}*/

void MainWindow::collision(){
    //On recupere les valeurs des infrarouges et on affiche si une valeur est supérieure à 100
    unsigned char  irAvG= robot->DataReceived[3];
    QString valeur_avg = QString::number(irAvG);
    int val_avg = valeur_avg.toInt();
    unsigned char irAvD = robot->DataReceived[11];
    QString valeur_avd = QString::number(irAvD);
    int val_avd = valeur_avd.toInt();
    unsigned char irArD = robot->DataReceived[4];
    QString valeur_ard = QString::number(irArD);
    int val_ard = valeur_ard.toInt();
    if ((val_avg > 100) && (val_avg > val_avd) && (val_avg > val_ard))
    {
        ui->collision->setText("Attention, obstacle detecté devant à gauche");
        stop();
    }
    else if ((val_avd > 100) && (val_avd > val_avg) && (val_avd > val_ard))
    {
        ui->collision->setText("Attention, obstacle detecté devant à droite");
    }
    else if ((val_ard > 100) && (val_ard > val_avg) && (val_ard > val_avd))
    {
        ui->collision->setText("Attention, obstacle detecté derriere à droite");
    }
    else
    {
        ui->collision->setText("aucun obstacle detecté");
    }

}

//TRAITEMENT D'IMAGE
void MainWindow::cam_filtre(int valeur)
{
    //On envoie des requêtes Javascript pour appliquer des filtres
    switch(valeur){
    case 1 :
        view->page()->runJavaScript("var filtres = document.body.firstChild.style.webkitFilter; document.body.firstChild.style.webkitFilter = filtres+' grayscale(100%)';");
        break;
    case 2 :
        view->page()->runJavaScript("var filtres = document.body.firstChild.style.webkitFilter; document.body.firstChild.style.webkitFilter = filtres+' invert(100%)';");
        break;
    case 3 :
        view->page()->runJavaScript("var filtres = document.body.firstChild.style.webkitFilter; document.body.firstChild.style.webkitFilter = filtres+' sepia(100%)';");
        break;
    case 4 :
        view->page()->runJavaScript("var filtres = document.body.firstChild.style.webkitFilter; document.body.firstChild.style.webkitFilter = filtres+' blur(5px)';");
        break;
    case 5 :
        view->page()->runJavaScript("var filtres = document.body.firstChild.style.webkitFilter; document.body.firstChild.style.webkitFilter = filtres+' saturate(200%)';");
        break;
    case 6 :
        view->page()->runJavaScript("var filtres = document.body.firstChild.style.webkitFilter; document.body.firstChild.style.webkitFilter = filtres+' brightness(50%)';");
        break;
    }

}
void MainWindow::cam_reset()
{
    view->page()->runJavaScript("document.body.firstChild.style.webkitFilter = ''");
}

//CAPTURE D'ECRAN

void MainWindow::screenshot(){
    QPixmap capt =QPixmap();
    capt =QPixmap::grabWidget(this->view);
    QString format = "png";
    QString initialPath = QDir::currentPath() + tr("/sans_nom.") + format;

        QString name = QFileDialog::getSaveFileName((QWidget*)this, tr("Save As"),
                                   initialPath,
                                   tr("%1 Files (*.%2);;All Files (*)")
                                   .arg(format.toUpper())
                                   .arg(format));
        if (!name.isEmpty()){
            capt.save(name, format.toUtf8());
        }
}


void MainWindow::on_screenshot_clicked()
{
   screenshot();
}

