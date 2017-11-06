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
    const float borderWidth = 2.f;

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
    const float ballSpeed = 200.f;

    //------------------------------------------------------------------------80
    // VARIABLES UTILES
    //------------------------------------------------------------------------80
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

    if (!ballSoundBuffer.loadFromFile(executablePath + "resources" + PATHSEP + "ball.wav"))
        return EXIT_FAILURE;

    sf::Sound ballSound(ballSoundBuffer);

    // Carga de las fuentes para el texto para los mensajes en la pantalla
    sf::Font font;
    if (!font.loadFromFile(executablePath + "resources" + PATHSEP + "sansation.ttf"))
        return EXIT_FAILURE;

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

    // ... y el espejo (donde se moverá su reflejo)
    sf::RectangleShape mirrorField = panelField;
    mirrorField.setPosition(panelWidth, windowHeight - panelHeight);


    // Creamos los obstáculos
    std::array<sf::RectangleShape, nObstacles> fieldObstacles;

    for (auto& obstacle : fieldObstacles) {
        obstacle.setSize(obstacleSize - sf::Vector2f(2*borderWidth, 2*borderWidth));
        obstacle.setOutlineThickness(borderWidth);
        obstacle.setOutlineColor(GreyD4);
        obstacle.setFillColor(PinkA2);
        obstacle.setOrigin(0.5f*obstacle.getSize());
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
    sf::CircleShape ball(ballRadius - borderWidth);
    ball.setOutlineThickness(borderWidth);
    ball.setOutlineColor(GreyD4);
    ball.setFillColor(Amber);
    ball.setOrigin(ballRadius, ballRadius);

    // Clones del circulo para las animaciones
    sf::CircleShape mirrorBall = ball;
    sf::CircleShape homoteticBall = ball;

    // Controlador de tiempo
    sf::Clock clock;

    // Estados
    bool isPlaying = false;
    bool isPause = true;

    bool homothecyEnabled = false;
    bool symmetryEnabled = false;
    bool rotationEnabled = false;
    bool colission = false;

    // Bucle principal de animacion
    while (window.isOpen()) {
        // REcivimos todos los eventos en el bucle de la animacion
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

                    // ball.setPosition(ballRadius, (panelHeight / 2) + panelField.getLocalBounds().top);
                    // ballAngle = 7*pi/4.f;

                    // ball.setPosition(panelWidth - ballRadius, (panelHeight / 2) + panelField.getLocalBounds().top);
                    // ballAngle = 5*pi/4.f;

                    // Elegimos el angulo de inicio
                    do {
                        ballAngle = static_cast<float>(angleDistribution() % 360) * pi / 180.f;
                    }
                    while (pi/3.f < ballAngle && ballAngle < 2.f*pi/3.f);
                }
                else {
                    // pausamos el programa
                    isPause = !isPause;
                    clock.restart();
                }
            }
        }

        if (isPlaying) {

            if(!isPause) {
                colission = false;

                float deltaTime = clock.restart().asSeconds();
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
                    // Si la pelota se acerca al cuadrado por la izquierda
                    // encontrará una pared hacia la derecha
                    xRight = obstacle.getPosition().x - obstacleSize.x/2;

                    // Si la pelota se acerca al cuadrado por la derecha
                    // encontrará una pared hacia la izquierda
                    xLeft = obstacle.getPosition().x + obstacleSize.x/2;

                    // Si la pelota se acerca al cuadrado por arriba
                    // encontrará una pared hacia abajo
                    yBottom = obstacle.getPosition().y - obstacleSize.y/2;

                    // Si la pelota se acerca al cuadrado por abajo
                    // encontrará una pared hacia arriba
                    yTop = obstacle.getPosition().y + obstacleSize.y/2;

                    // Si la pelota se acerca al cuadrado por la izquierda
                    // impactará en la cara izquierda del obtaculo, es decir en
                    // una pared hacia su derecha
                    if( ball.getPosition().x + ballRadius > xRight &&
                        ball.getPosition().x + ballRadius < obstacle.getPosition().x &&
                        ball.getPosition().y > obstacle.getPosition().y - obstacleSize.y / 2 &&
                        ball.getPosition().y < obstacle.getPosition().y + obstacleSize.y / 2
                    ) {
                        ballSound.play();
                        ballAngle = ((ballAngle < pi)?(1):(3))*pi - ballAngle;
                        xError = ball.getPosition().x + ballRadius - xRight;
                        ball.setPosition(ball.getPosition().x - 2*xError, ball.getPosition().y);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por la derecha
                    // impactará en la cara derecha del obtaculo, es decir en
                    // una pared hacia su izquierda
                    if( ball.getPosition().x - ballRadius < xLeft &&
                        ball.getPosition().x - ballRadius > obstacle.getPosition().x &&
                        ball.getPosition().y > obstacle.getPosition().y - obstacleSize.y / 2 &&
                        ball.getPosition().y < obstacle.getPosition().y + obstacleSize.y / 2
                    ) {
                        ballSound.play();
                        ballAngle = ((ballAngle < pi)?(1):(3))*pi - ballAngle;
                        xError = xLeft - ball.getPosition().x + ballRadius;
                        ball.setPosition(ball.getPosition().x + 2*xError, ball.getPosition().y);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por arriba
                    // impactará en la cara superior del obtaculo, es decir en
                    // una pared que se encuentra debajo
                    if( ball.getPosition().y + ballRadius > yBottom &&
                        ball.getPosition().y + ballRadius < obstacle.getPosition().y &&
                        ball.getPosition().x > obstacle.getPosition().x - obstacleSize.x / 2 &&
                        ball.getPosition().x < obstacle.getPosition().x + obstacleSize.x / 2
                    ) {
                        ballSound.play();
                        ballAngle = 2*pi - ballAngle;
                        yError = ball.getPosition().y + ballRadius - yBottom;
                        ball.setPosition(ball.getPosition().x, ball.getPosition().y - 2*yError);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por abajo
                    // impactará en la cara inferior del obtaculo, es decir en
                    // una pared que se encuentra por encima
                    if( ball.getPosition().y - ballRadius < yTop &&
                        ball.getPosition().y - ballRadius > obstacle.getPosition().y &&
                        ball.getPosition().x > obstacle.getPosition().x - obstacleSize.x / 2 &&
                        ball.getPosition().x < obstacle.getPosition().x + obstacleSize.x / 2
                    ) {
                        ballSound.play();
                        ballAngle = 2*pi - ballAngle;
                        yError = yTop - ball.getPosition().y + ballRadius;
                        ball.setPosition(ball.getPosition().x, ball.getPosition().y + 2*yError);
                        colission = true;
                    }

                    // Si la pelota se acerca al cuadrado por una esquina
                    // e impacta en ella
                    float xDistance = std::abs(obstacle.getPosition().x - ball.getPosition().x);
                    float yDistance = std::abs(obstacle.getPosition().y - ball.getPosition().y);
                    float cDistance = sqrtf(powf(xDistance - obstacleSize.x / 2.f, 2.f) + powf(yDistance - obstacleSize.y / 2.f, 2.f));
                    float h = 0.f;
                    float e = 0.f;
                    float b = 0.f;

                    if (cDistance < ballRadius) {
                        // La pelota llega por la esquina superior derecha del
                        // obstaculo
                        if(xRight >= ball.getPosition().x && yBottom >= ball.getPosition().y) {
                            // if(ballAngle == ballAngle) {
                            //     sf::Vertex line[] =
                            //     {
                            //         sf::Vertex(ball.getPosition() + 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle))),
                            //         sf::Vertex(ball.getPosition() - 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle))),
                            //     };
                            //     window.draw(line, 2, sf::Lines);
                            //     sf::Vertex line2[] =
                            //     {
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) + 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle)), sf::Color::Red),
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) - 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle)), sf::Color::Red),
                            //     };
                            //     window.draw(line2, 2, sf::Lines);
                            //     sf::Vertex line3[] =
                            //     {
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) + 100.f*(ball.getPosition() - sf::Vector2f(xLeft, yBottom)), sf::Color::Green),
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) - 100.f*(ball.getPosition() - sf::Vector2f(xLeft, yBottom)), sf::Color::Green),
                            //     };
                            //     window.draw(line3, 2, sf::Lines);
                            // }
                            ballSound.play();
                            h = xRight*std::sin(ballAngle) - yBottom*std::cos(ballAngle) + ball.getPosition().y*std::cos(ballAngle) - ball.getPosition().x*std::sin(ballAngle);
                            h = std::abs(h);
                            b = std::asin(h/ballRadius);
                            e = ballRadius*std::cos(b) - std::sqrt(cDistance*cDistance - h*h);
                            ball.setPosition(ball.getPosition().x - e*std::cos(ballAngle), ball.getPosition().y - e*std::sin(ballAngle));
                            ballAngle = pi + ballAngle + 2*b;
                            ball.setPosition(ball.getPosition().x + e*std::cos(ballAngle), ball.getPosition().y + e*std::sin(ballAngle));
                            colission = true;
                        }

                        // La pelota llega por la esquina superior izquieda del
                        // obstaculo
                        if(ball.getPosition().x >= xLeft && yBottom >= ball.getPosition().y) {
                            // if(ballAngle == ballAngle) {
                            //     sf::Vertex line[] =
                            //     {
                            //         sf::Vertex(ball.getPosition() + 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle))),
                            //         sf::Vertex(ball.getPosition() - 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle))),
                            //     };
                            //     window.draw(line, 2, sf::Lines);
                            //     sf::Vertex line2[] =
                            //     {
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) + 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle)), sf::Color::Red),
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) - 500.f * sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle)), sf::Color::Red),
                            //     };
                            //     window.draw(line2, 2, sf::Lines);
                            //     sf::Vertex line3[] =
                            //     {
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) + 100.f*(ball.getPosition() - sf::Vector2f(xLeft, yBottom)), sf::Color::Green),
                            //         sf::Vertex(sf::Vector2f(xLeft, yBottom) - 100.f*(ball.getPosition() - sf::Vector2f(xLeft, yBottom)), sf::Color::Green),
                            //     };
                            //     window.draw(line3, 2, sf::Lines);
                            // }
                            ballSound.play();
                            h = xLeft*std::sin(ballAngle) - yBottom*std::cos(ballAngle) + ball.getPosition().y*std::cos(ballAngle) - ball.getPosition().x*std::sin(ballAngle);
                            h = std::abs(h);
                            b = std::asin(h/ballRadius);
                            e = ballRadius*std::cos(b) - std::sqrt(cDistance*cDistance - h*h);
                            ball.setPosition(ball.getPosition().x - e*std::cos(ballAngle), ball.getPosition().y - e*std::sin(ballAngle));
                            ballAngle = pi + ballAngle + 2*b;
                            ball.setPosition(ball.getPosition().x + e*std::cos(ballAngle), ball.getPosition().y + e*std::sin(ballAngle));
                            colission = true;
                        }

                        // La pelota llega por la esquina inferior derecha del
                        // obstaculo
                        if(xLeft >= ball.getPosition().x && ball.getPosition().y >= yTop) {
                            colission = true;
                        }

                        // La pelota llega por la esquina inferior izquieda del
                        // obstaculo
                        if(ball.getPosition().x >= xRight && ball.getPosition().y >= yTop) {
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
            window.draw(mirrorField);
            window.draw(topSeparator);
            window.draw(verticalSeparator);

            for (auto& obstacle : fieldObstacles) {
                window.draw(obstacle);
            }

            for (auto& obstacle : mirrorObstacles) {
                window.draw(obstacle);
            }

            if(colission) {
                homothecyEnabled = false;
                symmetryEnabled = false;
                rotationEnabled = false;

                // Desactivar animaciones
                // ... aqui ...

                // Selecionar nueva animación
                int effect = effectDistribution() % 3;

                homothecyEnabled = (effect == 0);
                symmetryEnabled = (effect == 1);
                rotationEnabled = (effect == 2);
            };

            // Habilitar animaciones
            if(homothecyEnabled) {
                // Código de homotecia
                banner[1].setString(L"Homotecia");
                banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);
            }

            if(symmetryEnabled) {
                banner[1].setString(L"Simetría");
                banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);

                mirrorBall = ball;
                mirrorBall.setPosition(windowWidth - ball.getPosition().x, ball.getPosition().y);
                window.draw(mirrorBall);
            }

            if(rotationEnabled) {
                // Código de rotación
                banner[1].setString(L"Rotación");
                banner[1].setPosition(0.5f*(windowWidth - banner[1].getLocalBounds().width), 20);
            }

            window.draw(ball);

            // Textos del panel superior
            for (auto& animation: banner) {
                window.draw(animation);
            }
        }
        else {
            window.clear(Amber);

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
