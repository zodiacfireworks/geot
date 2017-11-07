//****************************************************************************80
//
//  PROGRAM    :
//               GeoT
//
//  PURPOSE    :
//               Program to show Goemetric Transformations as an applied
//               exploration SFML multimedia library.
//
//  PROGRAMMER :
//               Martín Josemaría <martin.vuelta@gmail.com>
//
//               * Software Development and Research
//                 SoftButterfly
//                 Lima - Peru
//
//               * Faculty of Physical Science
//                 Universidad Nacional Mayor de San Marcos
//                 Lima - Peru
//
//  FILE       :
//               main.cpp
//
//  DESCRIPTION:
//               This file contains the main functionalities of the program.
//
//****************************************************************************80

#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include <cmath>

#include <array>
#include <string>
#include <random>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #include <windows.h>
    const std::string PATHSEP = "\\";
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #include <unistd.h>
    const std::string PATHSEP = "/";
#else
    #error Unsupported or unknown operating system
#endif


//----------------------------------------------------------------------------80
//  FUNCIONES
//----------------------------------------------------------------------------80
std::string getExecutablePath();


//----------------------------------------------------------------------------80
//  FUNCION PRINCIPAL (MAIN)
//----------------------------------------------------------------------------80
int main() {

    //------------------------------------------------------------------------80
    //  CONSTANTES
    //------------------------------------------------------------------------80
    // Colores
    const sf::Color GreyD4(33, 33, 33, 255);
    const sf::Color GreyL4(245, 245, 245, 255);
    const sf::Color Amber(255, 193, 7, 255);
    const sf::Color PinkA2(255, 64, 129, 255);
    const sf::Color Red(244,67,54);

    // PI, para poder medir los angulos de direccion de movimiento de la
    // pelota usando radianes en fracciones de pi.
    const float pi = 3.14159265358979f;

    // Tamaño de la ventana de la aplciación
    const float windowWidth = 800.f;
    const float windowHeight = 600.f;

    // Tamaño de los paneles:
    // Se fedinirá un panel para el campo donde se desplaza la pelota
    // (fieldPanel) y otro en donde se mostrará la refelxion (mirrorPanel).
    const float panelWidth = windowWidth/2.f;
    const float panelHeight = 550.f;

    // Ancho de las líneas de separación entre los paneles
    const float separatorWidth = 2.f;
    // const float borderWidth = 2.f;

    // Número de obstaculos
    // Se usaran 4 obstaculos en posiciones fijas. Este valor se asigna en una
    // constante durante todo el programa, si cambia su valor aquí introducirá
    // errores pues siempre se asume que su valor es 4.
    const int nObstacles = 4;

    // Tamaño de los obstaculos 60x60
    const sf::Vector2f obstacleSize(60, 60);

    // Radio de la pelota
    const float ballRadius = 20.f;

    // Rapides de la pelota
    const float ballSpeed = 150.f;

    //------------------------------------------------------------------------80
    // VARIABLES UTILES
    //------------------------------------------------------------------------80
    // Tiempo acumulado para el efecto especial
    float time = 0.f;

    // Dirección de la pelota
    float ballAngle = 0.f;

    // Greneradores aleatorios para el angiulo inicial y la selecion de eventos
    std::random_device angleDistribution;
    std::random_device effectDistribution;

    //------------------------------------------------------------------------80
    // VEWNTANA DE LA APLICACIÓN
    //------------------------------------------------------------------------80
    // Ajustes de antialising (para que no se vea pixeleado)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 10;

    // Inicio de la ventana de la aplicación
    std::string programName = "GeoT";

    sf::RenderWindow window(
        sf::VideoMode(windowWidth, windowHeight, 32),
        programName,
        sf::Style::Titlebar | sf::Style::Close, settings
    );

    // Para "mejorar" la fecuencia de actualización de la pantalla
    window.setVerticalSyncEnabled(true);

    //------------------------------------------------------------------------80
    // RECURSOS EXTERNOS
    //------------------------------------------------------------------------80
    // Ruta del archivo ejecutable
    std::string executablePath = getExecutablePath();

    if (executablePath.length()>0) {
        executablePath += PATHSEP;
    }

    // Sonido de "boing!" cuando la Pelotita choca con los objetos y las
    // paredes
    sf::SoundBuffer ballSoundBuffer;

    if (!ballSoundBuffer.loadFromFile(executablePath + "resources" + PATHSEP + "ball.wav")) {
        return EXIT_FAILURE;
    }

    sf::Sound ballSound(ballSoundBuffer);

    // Carga de las fuentes para el texto para los mensajes en la pantalla
    sf::Font font;
    if (!font.loadFromFile(executablePath + "resources" + PATHSEP + "sansation.ttf")){
        return EXIT_FAILURE;
    }

    // Textura para la esfera
    sf::Texture ballTexture;
    if (!ballTexture.loadFromFile(executablePath + "resources" + PATHSEP + "sphere.png")) {
        return EXIT_FAILURE;
    }
    ballTexture.setSmooth(true);
    ballTexture.setRepeated(false);

    // Textura para los obstaculos
    sf::Texture brickTexture;
    if (!brickTexture.loadFromFile(executablePath + "resources" + PATHSEP + "brick.png")) {
        return EXIT_FAILURE;
    }
    brickTexture.setSmooth(true);
    brickTexture.setRepeated(false);

    // Textura para los paneles
    sf::Texture grassTexture;
    if (!grassTexture.loadFromFile(executablePath + "resources" + PATHSEP + "grass.png")) {
        return EXIT_FAILURE;
    }
    brickTexture.setSmooth(true);
    brickTexture.setRepeated(true);

    // Mensaje de bienvenida
    std::array<sf::Text, 4> welcomeMessage;

    for (auto& message: welcomeMessage) {
        message.setFont(font);
        message.setCharacterSize(20);
        message.setFillColor(GreyD4);
    }

    welcomeMessage[0].setPosition(100, 100);
    welcomeMessage[0].setString("Bienvenido a");

    welcomeMessage[1].setPosition(100, 120);
    welcomeMessage[1].setCharacterSize(80);
    welcomeMessage[1].setStyle(sf::Text::Bold);
    welcomeMessage[1].setString("GeoT");

    welcomeMessage[2].setPosition(100, 210);
    welcomeMessage[2].setString(L"Visulización de transformaciones geométricas.");

    welcomeMessage[3].setPosition(100, 240);
    welcomeMessage[3].setCharacterSize(12);
    welcomeMessage[3].setString("* Presiona la barra espaciadora para comenzar.");

    // Mensaje de animaciones en el banner (parte superior por encima de los
    // paneles)
    std::array<sf::Text, 2> banner;

    for (auto& coordinate: banner) {
        coordinate.setFont(font);
        coordinate.setCharacterSize(10);
        coordinate.setFillColor(GreyD4);
    }

    banner[0].setString(L"Animación activa");
    banner[0].setPosition(0.5f*(windowWidth - banner[0].getLocalBounds().width),5);

    banner[1].setString(L"Traslación");
    banner[1].setCharacterSize(25);
    banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);
    banner[1].setStyle(sf::Text::Bold);

    // Separadores de los paneles
    sf::RectangleShape topSeparator;
    topSeparator.setSize(sf::Vector2f(windowWidth, separatorWidth));
    topSeparator.setFillColor(GreyD4);
    topSeparator.setPosition(0, windowHeight - panelHeight - separatorWidth);

    sf::RectangleShape verticalSeparator;
    verticalSeparator.setSize(sf::Vector2f(separatorWidth, panelHeight));
    verticalSeparator.setFillColor(GreyD4);
    verticalSeparator.setPosition(panelWidth - separatorWidth/2.f, windowHeight - panelHeight);

    // Creamos el campo (donde se movera la pelotita) ...
    sf::RectangleShape panelField;
    panelField.setSize(sf::Vector2<float>(panelWidth, panelHeight));
    panelField.setFillColor(GreyL4);
    panelField.setPosition(0, windowHeight - panelHeight);
    panelField.setTexture(&grassTexture);

    // ... y el espejo (donde se moverá su reflejo)
    sf::RectangleShape mirrorField = panelField;
    mirrorField.setPosition(panelWidth, windowHeight - panelHeight);

    // Creamos los obstáculos
    std::array<sf::RectangleShape, nObstacles> fieldObstacles;

    for (auto& obstacle : fieldObstacles) {
        obstacle.setSize(obstacleSize);
        obstacle.setOrigin(0.5f*obstacle.getSize());
        obstacle.setTexture(&brickTexture);
    }

    fieldObstacles[0].setPosition(panelWidth/3.f, panelHeight/3.f + (windowHeight - panelHeight));
    fieldObstacles[1].setPosition(panelWidth/3.f, 2*panelHeight/3.f + (windowHeight - panelHeight));
    fieldObstacles[2].setPosition(2*panelWidth/3.f, panelHeight/3.f + (windowHeight - panelHeight));
    fieldObstacles[3].setPosition(2*panelWidth/3.f, 2*panelHeight/3.f + (windowHeight - panelHeight));

    std::array<sf::RectangleShape, nObstacles> mirrorObstacles = fieldObstacles;

    mirrorObstacles[0].setPosition(windowWidth - fieldObstacles[0].getPosition().x, fieldObstacles[0].getPosition().y);
    mirrorObstacles[1].setPosition(windowWidth - fieldObstacles[1].getPosition().x, fieldObstacles[1].getPosition().y);
    mirrorObstacles[2].setPosition(windowWidth - fieldObstacles[2].getPosition().x, fieldObstacles[2].getPosition().y);
    mirrorObstacles[3].setPosition(windowWidth - fieldObstacles[3].getPosition().x, fieldObstacles[3].getPosition().y);

    // Circulo movil
    sf::CircleShape ball(ballRadius);
    // ball.setOutlineThickness(borderWidth);
    // ball.setOutlineColor(GreyD4);
    // ball.setFillColor(Amber);
    ball.setOrigin(ballRadius, ballRadius);
    ball.setTexture(&ballTexture);

    // Clones del circulo para las animaciones
    sf::CircleShape mirrorBall = ball;
    sf::CircleShape homoteticBall(ballRadius);
    homoteticBall.setFillColor(Amber);

    sf::Vertex homoteticAxis[2] = {
        sf::Vertex(sf::Vector2f(0,0), Red),
        sf::Vertex(sf::Vector2f(0,0), Red)
    };
    homoteticAxis[0] = ball.getPosition();

    // Controlador de tiempo
    sf::Clock clock;

    // Estados
    bool isPlaying = false;
    bool isPause = true;

    bool homothecyEnabled = false;
    bool symmetryEnabled = false;
    bool rotationEnabled = false;
    bool colission = false;

    // Effecto especial
    bool specialEffect = false;

    std::array<sf::Vector2f, nObstacles> angularVelocities;
    angularVelocities[0] = sf::Vector2f(static_cast<float>(angleDistribution() % 90), static_cast<float>(angleDistribution() % 90)) / 90.f;
    angularVelocities[1] = sf::Vector2f(static_cast<float>(angleDistribution() % 90), static_cast<float>(angleDistribution() % 90)) / 90.f;
    angularVelocities[2] = sf::Vector2f(static_cast<float>(angleDistribution() % 90), static_cast<float>(angleDistribution() % 90)) / 90.f;
    angularVelocities[3] = sf::Vector2f(static_cast<float>(angleDistribution() % 90), static_cast<float>(angleDistribution() % 90)) / 90.f;

    std::array<float, nObstacles> phases;
    phases[0] = static_cast<float>(angleDistribution() % 10)*pi/40;
    phases[1] = static_cast<float>(angleDistribution() % 10)*pi/40;
    phases[2] = static_cast<float>(angleDistribution() % 10)*pi/40;
    phases[3] = static_cast<float>(angleDistribution() % 10)*pi/40;

    // Bucle principal de animación
    while (window.isOpen()) {
        // Recivimos todos los eventos en el bucle de la animacion
        sf::Event event;

        while (window.pollEvent(event)) {
            // "Window closed" o "ESC": exit (salir)
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                window.close();
                break;
            }

            // "SAPCE": Inicia la animación (iniciar o pausar)
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space)) {
                if (!isPlaying) {
                    // (re)inicar la animacion
                    isPlaying = true;
                    isPause = false;
                    clock.restart();

                    ball.setPosition((panelWidth / 2), (panelHeight / 2) + (windowHeight - panelHeight));

                    // Elegimos el ángulo de inicio
                    do {
                        ballAngle = static_cast<float>(angleDistribution() % 360) * pi / 180.f;
                    }
                    while (ballAngle < pi/3.f || (2.f*pi/3.f < ballAngle && ballAngle < 4.f*pi/3.f) || 5*pi/3.f < ballAngle);
                }
                else {
                    // pausamos el programa
                    isPause = !isPause;
                    clock.restart();
                }
            }

            // activamos el efecto especial
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R)) {
                if (isPlaying && !isPause) {
                    specialEffect = !specialEffect;

                    // if(!specialEffect) {
                    //     time = 0.f;
                    // }
                }
            }
        }

        if (isPlaying) {
            if(!isPause) {
                colission = false;

                float deltaTime = clock.restart().asSeconds();

                if(specialEffect) {
                    time += deltaTime;

                    fieldObstacles[0].setPosition(
                        30.f*std::sin(10.f*angularVelocities[0].x*time + phases[0]) + panelWidth/3.f,
                        30.f*std::sin(10.f*angularVelocities[0].y*time) + panelHeight/3.f + (windowHeight - panelHeight)
                    );
                    fieldObstacles[1].setPosition(
                        30.f*std::sin(10.f*angularVelocities[1].x*time + phases[1]) + panelWidth/3.f,
                        30.f*std::sin(10.f*angularVelocities[1].y*time) + 2*panelHeight/3.f + (windowHeight - panelHeight)
                    );
                    fieldObstacles[2].setPosition(
                        30.f*std::sin(10.f*angularVelocities[2].x*time + phases[2]) + 2*panelWidth/3.f,
                        30.f*std::sin(10.f*angularVelocities[2].y*time) + panelHeight/3.f + (windowHeight - panelHeight)
                    );
                    fieldObstacles[3].setPosition(
                        30.f*std::sin(10.f*angularVelocities[3].x*time + phases[3]) + 2*panelWidth/3.f,
                        30.f*std::sin(10.f*angularVelocities[3].y*time) + 2*panelHeight/3.f + (windowHeight - panelHeight)
                    );
                }

                float yError = 0;
                float xError = 0;

                float xLeft = 0;
                float xRight = panelWidth;
                float yTop = windowHeight - panelHeight;
                float yBottom = windowHeight;

                // Movemos la bolita
                float factor = ballSpeed * deltaTime;

                ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

                // Verificamos choques con los extremos de la pantalla

                // Si hay impacto con el borde izquierdo
                if(ball.getPosition().x - ballRadius < xLeft) {
                    ballSound.play();
                    ballAngle = ((ballAngle < pi)?(1):(3))*pi - ballAngle;
                    xError = xLeft - ball.getPosition().x + ballRadius;
                    ball.setPosition(ball.getPosition().x + 2*xError, ball.getPosition().y);
                    colission = true;
                }

                // Si hay impacto con el borde derecho
                if(ball.getPosition().x + ballRadius > xRight) {
                    ballSound.play();
                    ballAngle = ((ballAngle < pi)?(1):(3))*pi - ballAngle;
                    xError = ball.getPosition().x + ballRadius - xRight;
                    ball.setPosition(ball.getPosition().x - 2*xError, ball.getPosition().y);
                    colission = true;
                }

                // Si hay impacto con el borde superior
                if(ball.getPosition().y - ballRadius < yTop) {
                    ballSound.play();
                    ballAngle = 2*pi - ballAngle;
                    yError = yTop - ball.getPosition().y + ballRadius;
                    ball.setPosition(ball.getPosition().x, ball.getPosition().y + 2*yError);
                    colission = true;
                }

                // Si hay impacto con el borde inferior
                if(ball.getPosition().y + ballRadius > yBottom) {
                    ballSound.play();
                    ballAngle = 2*pi - ballAngle;
                    yError = ball.getPosition().y + ballRadius - yBottom;
                    ball.setPosition(ball.getPosition().x, ball.getPosition().y - 2*yError);
                    colission = true;
                }

                // Verificamos choques con los obstaculos
                for(auto& obstacle: fieldObstacles) {
                    // Lado izquierdo del obstáculo
                    xLeft = obstacle.getPosition().x - obstacleSize.x/2;

                    // Lado derecho del obstáculo
                    xRight = obstacle.getPosition().x + obstacleSize.x/2;

                    // Lado superior del obstáculo
                    yTop = obstacle.getPosition().y - obstacleSize.y/2;

                    // Lado inferior del obstáculo
                    yBottom = obstacle.getPosition().y + obstacleSize.y/2;

                    // Si la pelota se acerca al cuadrado por la izquierda
                    if( ball.getPosition().x + ballRadius > xLeft &&
                        ball.getPosition().x + ballRadius < obstacle.getPosition().x &&
                        ball.getPosition().y >= yTop &&
                        ball.getPosition().y <= yBottom
                    ) {
                        ballSound.play();
                        ballAngle = ((ballAngle < pi)?(1):(3))*pi - ballAngle;
                        xError = ball.getPosition().x + ballRadius - xLeft;
                        ball.setPosition(ball.getPosition().x - 2*xError, ball.getPosition().y);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por la derecha
                    if( ball.getPosition().x - ballRadius < xRight &&
                        ball.getPosition().x - ballRadius > obstacle.getPosition().x &&
                        ball.getPosition().y >= yTop &&
                        ball.getPosition().y <= yBottom
                    ) {
                        ballSound.play();
                        ballAngle = ((ballAngle < pi)?(1):(3))*pi - ballAngle;
                        xError = xRight - ball.getPosition().x + ballRadius;
                        ball.setPosition(ball.getPosition().x + 2*xError, ball.getPosition().y);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por arriba
                    if( ball.getPosition().y + ballRadius > yTop &&
                        ball.getPosition().y + ballRadius < obstacle.getPosition().y &&
                        ball.getPosition().x >= xLeft &&
                        ball.getPosition().x <= xRight
                    ) {
                        ballSound.play();
                        ballAngle = 2*pi - ballAngle;
                        yError = ball.getPosition().y + ballRadius - yTop;
                        ball.setPosition(ball.getPosition().x, ball.getPosition().y - 2*yError);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por abajo
                    if( ball.getPosition().y - ballRadius < yBottom &&
                        ball.getPosition().y - ballRadius > obstacle.getPosition().y &&
                        ball.getPosition().x >= xLeft &&
                        ball.getPosition().x <= xRight
                    ) {
                        ballSound.play();
                        ballAngle = 2*pi - ballAngle;
                        yError = yBottom - ball.getPosition().y + ballRadius;
                        ball.setPosition(ball.getPosition().x, ball.getPosition().y + 2*yError);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por una esquina
                    // e impacta en ella
                    float xDistance = std::abs(obstacle.getPosition().x - ball.getPosition().x);
                    float yDistance = std::abs(obstacle.getPosition().y - ball.getPosition().y);
                    float cDistance = std::sqrt(std::pow(xDistance - obstacleSize.x / 2.f, 2.f) + std::pow(yDistance - obstacleSize.y / 2.f, 2.f));
                    float h = 0.f;
                    float e = 0.f;
                    float b = 0.f;

                    if (cDistance < ballRadius) {
                        /*
                        if(ball.getPosition().x < xLeft && ball.getPosition().y < yTop) {
                            sf::Vector2f C = {xLeft, yTop};
                            sf::Vector2f P = ball.getPosition();

                            sf::Vector2f CP = P - C;

                            float R = ballRadius;
                            float theta = ballAngle;

                            float b = (CP.x*std::cos(theta) + CP.y*std::sin(theta));
                            float d = std::sqrt(b*b + R*R - CP.x*CP.x - CP.y*CP.y);

                            float t = b + d;

                            sf::Vector2f O = P - t*sf::Vector2f(std::cos(theta), std::sin(theta));

                            if (O.x >= xLeft || O.y >= yTop) {

                            }
                            float t = b - d;
                        }
                        */

                        // La pelota llega por la esquina superior izquierda
                        if(ball.getPosition().x < xLeft && ball.getPosition().y < yTop) {
                            ballSound.play();
                            h = xLeft*std::sin(ballAngle) - yTop*std::cos(ballAngle) + ball.getPosition().y*std::cos(ballAngle) - ball.getPosition().x*std::sin(ballAngle);
                            h = std::abs(h);

                            std::cout << "-- Impacto --" << std::endl;
                            std::cout << "Parámetros del obstáculo" << std::endl;
                            std::cout << "xLeft:   " << xLeft   << std::endl;
                            std::cout << "xRight:  " << xRight  << std::endl;
                            std::cout << "yTop:    " << yTop    << std::endl;
                            std::cout << "yBottom: " << yBottom << std::endl;
                            std::cout << "Parámetros de la pelota" << std::endl;
                            std::cout << "X    : " << ball.getPosition().x << std::endl;
                            std::cout << "Y    : " << ball.getPosition().y << std::endl;
                            std::cout << "Angle: " << ballAngle*180/pi << std::endl;
                            std::cout << "Distancia del centro de la esfera a la recta de impacto" << std::endl;
                            std::cout << "h = |"
                                      << xRight << "*" << std::sin(ballAngle) << "-"
                                      << yTop << "*" << std::cos(ballAngle) << "+"
                                      << ball.getPosition().y << "*" << std::cos(ballAngle) << "-"
                                      << ball.getPosition().x << "*" << std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = |"
                                      << xRight*std::sin(ballAngle) << "-"
                                      << yTop*std::cos(ballAngle) << "+"
                                      << ball.getPosition().y*std::cos(ballAngle) << "-"
                                      << ball.getPosition().x*std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = " << h << std::endl;

                            b = std::asin(h/ballRadius);
                            e = ballRadius*std::cos(b) - std::sqrt(cDistance*cDistance - h*h);
                            ball.setPosition(ball.getPosition().x - e*std::cos(ballAngle), ball.getPosition().y - e*std::sin(ballAngle));

                            if(0 <= ballAngle && ballAngle < pi/2) {
                                ballAngle = ballAngle + 2*b + pi;
                            }
                            if (pi/2 <= ballAngle && ballAngle < pi) {
                                ballAngle = pi + ballAngle - 2*b;
                            }
                            // if (pi <= ballAngle && ballAngle < 3*pi/2) {
                            //     ballAngle = pi + ballAngle - 2*b;
                            // }
                            if (3*pi/2 <= ballAngle && ballAngle < 2*pi) {
                                ballAngle = ballAngle + 2*b - pi;
                            }

                            std::cout << "Angulo de corte : " << b*180/pi << std::endl;
                            std::cout << "Angulo de rebote: " << ballAngle*180/pi << std::endl;

                            ball.setPosition(ball.getPosition().x + e*std::cos(ballAngle), ball.getPosition().y + e*std::sin(ballAngle));
                            colission = true;
                        }

                        // La pelota llega por la esquina inferior izquieda
                        if(ball.getPosition().x < xLeft && ball.getPosition().y > yBottom) {
                            ballSound.play();
                            h = xLeft*std::sin(ballAngle) - yBottom*std::cos(ballAngle) + ball.getPosition().y*std::cos(ballAngle) - ball.getPosition().x*std::sin(ballAngle);
                            h = std::abs(h);

                            std::cout << "-- Impacto --" << std::endl;
                            std::cout << "Parámetros del obstáculo" << std::endl;
                            std::cout << "xLeft:   " << xLeft   << std::endl;
                            std::cout << "xRight:  " << xRight  << std::endl;
                            std::cout << "yTop:    " << yTop    << std::endl;
                            std::cout << "yBottom: " << yBottom << std::endl;
                            std::cout << "Parámetros de la pelota" << std::endl;
                            std::cout << "X    : " << ball.getPosition().x << std::endl;
                            std::cout << "Y    : " << ball.getPosition().y << std::endl;
                            std::cout << "Angle: " << ballAngle*180/pi << std::endl;
                            std::cout << "Distancia del centro de la esfera a la recta de impacto" << std::endl;
                            std::cout << "h = |"
                                      << xRight << "*" << std::sin(ballAngle) << "-"
                                      << yTop << "*" << std::cos(ballAngle) << "+"
                                      << ball.getPosition().y << "*" << std::cos(ballAngle) << "-"
                                      << ball.getPosition().x << "*" << std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = |"
                                      << xRight*std::sin(ballAngle) << "-"
                                      << yTop*std::cos(ballAngle) << "+"
                                      << ball.getPosition().y*std::cos(ballAngle) << "-"
                                      << ball.getPosition().x*std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = " << h << std::endl;

                            b = std::asin(h/ballRadius);
                            e = ballRadius*std::cos(b) - std::sqrt(cDistance*cDistance - h*h);
                            ball.setPosition(ball.getPosition().x - e*std::cos(ballAngle), ball.getPosition().y - e*std::sin(ballAngle));

                            if(0 <= ballAngle && ballAngle < pi/2) {
                                ballAngle = pi + ballAngle - 2*b;
                            }
                            else if (pi/2 <= ballAngle && ballAngle < 3*pi/2) {
                                ballAngle = pi + ballAngle + 2*b;
                            } else {
                                ballAngle = ballAngle - pi + 2*b;
                            }

                            std::cout << "Angulo de corte : " << b*180/pi << std::endl;
                            std::cout << "Angulo de rebote: " << ballAngle*180/pi << std::endl;

                            ball.setPosition(ball.getPosition().x + e*std::cos(ballAngle), ball.getPosition().y + e*std::sin(ballAngle));
                            colission = true;
                        }

                        // La pelota llega por la esquina inferior derecha
                        if(ball.getPosition().x > xRight && ball.getPosition().y > yBottom) {
                            ballSound.play();
                            h = xRight*std::sin(ballAngle) - yBottom*std::cos(ballAngle) + ball.getPosition().y*std::cos(ballAngle) - ball.getPosition().x*std::sin(ballAngle);
                            h = std::abs(h);

                            std::cout << "-- Impacto --" << std::endl;
                            std::cout << "Parámetros del obstáculo" << std::endl;
                            std::cout << "xLeft:   " << xLeft   << std::endl;
                            std::cout << "xRight:  " << xRight  << std::endl;
                            std::cout << "yTop:    " << yTop    << std::endl;
                            std::cout << "yBottom: " << yBottom << std::endl;
                            std::cout << "Parámetros de la pelota" << std::endl;
                            std::cout << "X    : " << ball.getPosition().x << std::endl;
                            std::cout << "Y    : " << ball.getPosition().y << std::endl;
                            std::cout << "Angle: " << ballAngle*180/pi << std::endl;
                            std::cout << "Distancia del centro de la esfera a la recta de impacto" << std::endl;
                            std::cout << "h = |"
                                      << xRight << "*" << std::sin(ballAngle) << "-"
                                      << yTop << "*" << std::cos(ballAngle) << "+"
                                      << ball.getPosition().y << "*" << std::cos(ballAngle) << "-"
                                      << ball.getPosition().x << "*" << std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = |"
                                      << xRight*std::sin(ballAngle) << "-"
                                      << yTop*std::cos(ballAngle) << "+"
                                      << ball.getPosition().y*std::cos(ballAngle) << "-"
                                      << ball.getPosition().x*std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = " << h << std::endl;

                            b = std::asin(h/ballRadius);
                            e = ballRadius*std::cos(b) - std::sqrt(cDistance*cDistance - h*h);
                            ball.setPosition(ball.getPosition().x - e*std::cos(ballAngle), ball.getPosition().y - e*std::sin(ballAngle));

                            if(0 <= ballAngle && ballAngle < pi/2) {
                                ballAngle = pi + ballAngle + 2*b;
                            }
                            else if (pi/2 <= ballAngle && ballAngle < 3*pi/2) {
                                ballAngle = pi + ballAngle - 2*b;
                            } else {
                                ballAngle = ballAngle - pi - 2*b;
                            }

                            std::cout << "Angulo de corte : " << b*180/pi << std::endl;
                            std::cout << "Angulo de rebote: " << ballAngle*180/pi << std::endl;

                            ball.setPosition(ball.getPosition().x + e*std::cos(ballAngle), ball.getPosition().y + e*std::sin(ballAngle));
                            colission = true;
                        }

                        // La pelota llega por la esquina superior derecha
                        if(ball.getPosition().x > xRight && ball.getPosition().y < yTop) {
                            ballSound.play();

                            h = xRight*std::sin(ballAngle) - yTop*std::cos(ballAngle) + ball.getPosition().y*std::cos(ballAngle) - ball.getPosition().x*std::sin(ballAngle);

                            h = std::abs(h);

                            std::cout << "-- Impacto --" << std::endl;
                            std::cout << "Parámetros del obstáculo" << std::endl;
                            std::cout << "xLeft:   " << xLeft   << std::endl;
                            std::cout << "xRight:  " << xRight  << std::endl;
                            std::cout << "yTop:    " << yTop    << std::endl;
                            std::cout << "yBottom: " << yBottom << std::endl;
                            std::cout << "Parámetros de la pelota" << std::endl;
                            std::cout << "X    : " << ball.getPosition().x << std::endl;
                            std::cout << "Y    : " << ball.getPosition().y << std::endl;
                            std::cout << "Angle: " << ballAngle << std::endl;
                            std::cout << "Distancia del centro de la esfera a la recta de impacto" << std::endl;
                            std::cout << "h = |"
                                      << xRight << "*" << std::sin(ballAngle) << "-"
                                      << yTop << "*" << std::cos(ballAngle) << "+"
                                      << ball.getPosition().y << "*" << std::cos(ballAngle) << "-"
                                      << ball.getPosition().x << "*" << std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = |"
                                      << xRight*std::sin(ballAngle) << "-"
                                      << yTop*std::cos(ballAngle) << "+"
                                      << ball.getPosition().y*std::cos(ballAngle) << "-"
                                      << ball.getPosition().x*std::sin(ballAngle)
                                      << "|" << std::endl;
                            std::cout << "h = " << h << std::endl;

                            b = std::asin(h/ballRadius);
                            e = ballRadius*std::cos(b) - std::sqrt(cDistance*cDistance - h*h);
                            ball.setPosition(ball.getPosition().x - e*std::cos(ballAngle), ball.getPosition().y - e*std::sin(ballAngle));

                            if(ballAngle <= pi) {
                                ballAngle = pi + ballAngle + 2*b;
                            }
                            else {
                                ballAngle = pi + ballAngle - 2*b;
                            }

                            ball.setPosition(ball.getPosition().x + e*std::cos(ballAngle), ball.getPosition().y + e*std::sin(ballAngle));
                            colission = true;
                        }
                    }
                }
            }
        }

        // Limpiamos la pantalla
        window.clear(Amber);

        if (isPlaying) {
            // Dibujar páneles, obstaculos, la esfra y los efectos
            // de transformación
            window.draw(panelField);

            if(colission) {
                if(!isPause) {
                    homothecyEnabled = false;
                    symmetryEnabled = false;
                    rotationEnabled = false;

                    // Desactivar animaciones
                    homoteticBall.setRadius(ballRadius);
                    homoteticAxis[0].position = sf::Vector2f(0,0);
                    homoteticAxis[1].position = sf::Vector2f(0,0);

                    // Selecionar nueva animación
                    int effect = effectDistribution() % 3;

                    homothecyEnabled = (effect == 0);
                    symmetryEnabled = (effect == 1);
                    rotationEnabled = (effect == 2);
                }
            }

            // Habilitar animaciones
            if(homothecyEnabled) {
                // Código de homotecia
                banner[1].setString(L"Homotecia");
                banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);

                if (!isPause) {
                    // homoteticBall = ball;
                    homoteticBall.setFillColor(Amber);
                    // sf::Vector2f scale = homoteticBall.getScale();
                    // homoteticBall.setPosition(ball.getPosition().x, ball.getPosition().y);
                    // homoteticBall.scale(1.01f, 1.01f);
                    float homoteticRadius = homoteticBall.getRadius();
                    homoteticRadius += 0.5f;
                    homoteticBall.setRadius(homoteticRadius);
                    homoteticBall.setOrigin(homoteticRadius, homoteticRadius);
                    homoteticBall.setPosition(ball.getPosition().x, ball.getPosition().y);

                    if (homoteticAxis[0].position == sf::Vector2f(0,0)) {
                        homoteticAxis[0].position = ball.getPosition();
                    }
                    homoteticAxis[1].position = ball.getPosition();

                    window.draw(homoteticBall);
                }
            }

            if(rotationEnabled) {
                // Código de rotación
                banner[1].setString(L"Rotación");
                banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);

                if (!isPause) {
                    ball.rotate(5);
                }
            }

            window.draw(mirrorField);
            window.draw(topSeparator);
            window.draw(verticalSeparator);

            for (auto& obstacle : mirrorObstacles) {
                window.draw(obstacle);
            }

            window.draw(ball);

            if(homothecyEnabled) {
                if (!isPause) {
                    window.draw(homoteticAxis, 2, sf::Lines);
                }
            }

            if (specialEffect) {
                mirrorObstacles[0].setPosition(windowWidth - fieldObstacles[0].getPosition().x, fieldObstacles[0].getPosition().y);
                mirrorObstacles[1].setPosition(windowWidth - fieldObstacles[1].getPosition().x, fieldObstacles[1].getPosition().y);
                mirrorObstacles[2].setPosition(windowWidth - fieldObstacles[2].getPosition().x, fieldObstacles[2].getPosition().y);
                mirrorObstacles[3].setPosition(windowWidth - fieldObstacles[3].getPosition().x, fieldObstacles[3].getPosition().y);
            }

            for (auto& obstacle : fieldObstacles) {
                window.draw(obstacle);
            }

            if(symmetryEnabled) {
                banner[1].setString(L"Simetría");
                banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);

                if (!isPause) {
                    mirrorBall = ball;
                    mirrorBall.setPosition(windowWidth - ball.getPosition().x, ball.getPosition().y);
                    window.draw(mirrorBall);
                }
            }

            // Textos del panel superior
            for (auto& animation: banner) {
                window.draw(animation);
            }
        }
        else {
            // Imprimir el mensaje de bienvenida
            for (auto& message: welcomeMessage) {
                window.draw(message);
            }
        }

        // Fin del cuadro de animacion actual
        window.display();
    }

    return 0;
}

std::string getExecutablePath() {
    std::string executablePath = "";
    char pBuf[255];
    int len = 255;
    int bytes;

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
        bytes = GetModuleFileName(NULL, pBuf, len);

        if (bytes != 0) {
            executablePath = std::string(pBuf);
        }
    #elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
        char szTmp[32];

        sprintf(szTmp, "/proc/%d/exe", getpid());
        bytes = std::min<int>(static_cast<int>(readlink(szTmp, pBuf, len)), len - 1);

        if (bytes >= 0) {
            pBuf[bytes] = '\0';
            executablePath = std::string(pBuf);
        }
    #else
        #error Unsupported or unknown operating system
    #endif

    if (executablePath.length()>0) {
        executablePath = executablePath.substr(0, executablePath.find_last_of("\\/"));
    }

    return executablePath;
}
