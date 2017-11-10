# GeoT

Programa de visaulización de transformaciones geométricas.

## Como usar

### Windows

* Descargar los compiladores GNU de la distribución TDM-GCC

```
https://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/Previous/1.1309.0/tdm64-gcc-4.9.2-3.exe/download
```

* Ejecutar el instalador.

* Desmarcar la opción de **Check for updates ...**.

* Selecionar la opción **Create**.

* Seguir con la instalacion por defecto. Anotar el nombre de la carpeta de
instalación

* Descargar la libreria SFML desde el siguiente enlace:

```
https://www.sfml-dev.org/files/SFML-2.4.2-windows-gcc-4.9.2-tdm-64-bit.zip
```

* Descomprimir el archivo descargado

* Copiar el contenido de la carpeta `SFML-2.4.2` en la carpeta de instalación de
TDM-GCC.

* Abrir una terminal de TDM-GCC y navegar hasta el folder donde se encuentra el
archivo Makefile del proyecto.

* Para compilar el programa, ejecutar el comando siguiente

```
mingw32-make
```

* Luego ingresar a la carpeta build y abrirá el programa.

* **Importante**: El programa se crea junto con una carpeta llamada `resources`
esta carpeta y el programa siempre debe de permanecer juntos.
