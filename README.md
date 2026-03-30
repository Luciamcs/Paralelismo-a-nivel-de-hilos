# Práctica 4 - Paralelismo con OpenMP y Programación Asíncrona

Este repositorio contiene el desarrollo de la Práctica 4 de la asignatura Computación de Alto Rendimiento del Grado en Ingeniería en Inteligencia Artificial (Universidad de Alicante).


## Descripción

El objetivo de esta práctica es aplicar técnicas de paralelización a nivel de hilos utilizando:

OpenMP (paralelismo de datos)
std::async en C++ (paralelismo funcional/asíncrono)

Todo ello se implementa sobre un problema real: el análisis forense de manipulación de imágenes digitales, donde se detectan posibles alteraciones en imágenes mediante diferentes técnicas.

Según el enunciado, el sistema se ejecuta en una arquitectura de memoria compartida, donde todos los hilos acceden a la misma memoria
