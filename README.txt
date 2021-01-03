##### Definitivo

* Una solución inicial (SI para abreviar) se crea de la siguiente forma: 
    1. Se elige la posición de un evento cualquiera PA 
    2. Se agrega PA a SI. 
    3. Se guarda la información de PA (coordenadas) --> PA = posición actual
    4. Se elige una nueva posición PS cualquiera    --> PS = posición siguiente
    5. Se verifica si cumple el criterio de la función miope
        5.1. Si lo cumple, se agrega a SI. Actualizo PA <= PS
        5.2. Si no, se ignora
    6. Repetir desde (4.) hasta que el tamaño de SI llegue a cierto umbral.

* La función miope verifica que PS se encuentre a "x" unidades (o más) de distancia de PA,
siendo "x" el radio de cobertura entregado por la instancia. Notar que si se repite una 
posición escogida, el algoritmo la agrega a SI, pero como SI es un conjunto esta acción
no tiene efecto alguno puesto que el tamaño de SI no varía, asegurando que siempre se 
tenga una cantidad inicial definida de AED instalados en la solución inicial.

#### Borrador
Supuestos necesarios en la ejecución
1. Las instancias estarán en el mismo directorio que el codigo fuente (solo por facilidad). Aunque mientras la ruta
ingresada esté tipeada correctamente, no debería tener problemas.
2. Para mi, el presupuesto solo restringe al algoritmo de HC, es decir, restringe la cantidad de AED nuevos que 
instalemos o movamos. En otras palabras, el presupuesto restringe la reparación de la solución inicial, pero no 
restringe al algoritmo Greedy que crea esa solución inicial. Poniendo un caso simple: hay 100 ubicaciones, mi presupuesto
es 5. Creo una solucion con 7 AED y de ahí los muevo o instalo otros :v le woa a preguntar al usuario cuantos quiere

Estrategias usadas en el algoritmo
* (Borrador) Se puede tener un array por cada dato en las lineas del archivo, o sea, un array de coordenadas x,
otro array de coordenadas y, y otro de si tiene un AED o no. La idea es facilitar el acceso a estas variables 
para calcular la función de evaluación.

*La función de evaluación me la dan: es la cantidad de eventos cubiertos. Entonces la idea es tener un contador
de eventos cubiertos que se vaya actualizando. Pero esto es problemático si existen overlaps entre AED. Bajo esa lógica,
podría tener un objeto/estructura de tipo conjunto, tal que si un evento está cubierto, entra al conjunto. Y si por alguna
razón, el evento deja de estar cubierto, sale del conjunto. De tal manera que al finalizar la iteración, el óptimo local 
será el mayor tamaño que haya obtenido el conjunto. Ojo: podría tener un array que sume en cada casilla +1 si algún 
vecino tiene un AED o reste -1 si un vecino pierde un AED. De esta manera puedo saber cuántos eventos han sido cubiertos
recorriendo este array con un for y preguntando si el valor de la casilla es mayor que 0.

*¿Cómo manejo restricciones? Bueno, hay 2 restricciones básicas. 
    1. Solo 1 AED por posición 
    2. La cantidad de AED instalados o movidos debe costar menos que el presupuesto
(1.) se puede manejar a través de la representación. Un array que por cada índice i, tenga un 1 (posee AED)
o un 0 (No posee AED). Esto va de la mano con la idea de tener 3 array, porque así por cada índice i, yo conozco su 
posición x del array 1, su posición y del array 2 y con esto puedo obtener sus vecinos

Guía de trabajo
- Prólogo: ordenar los datos que me están entregando. Vienen en un txt y son lineas con coordenadas y un booleano.
No tengo más información que la entregada por parámetros.

- Primero necesito crear una solución inicial, ¿qué es una solución inicial? Es una disposición de AED instalados
siguiendo una regla de construcción, ¿qué regla de construcción? La función miope del algoritmo Greedy. ¿Cuál es? 
Es una función que toma una ubicacion "U" cualquiera, luego elige otra posición "V" y la añade si es que está fuera
del radio de cobertura. Entonces mi solución corresponde a un subconjunto de las posiciones iniciales ¿Cómo represento
tal conjunto? Puede ser literalmente un set, un array, un vector. Ya tengo el array que guarda si una posición tiene
un AED instalado, es mi candidata a solución inicial. En realidad puedo trabajar ese array reiniciandolo en cada 
iteracion.

- Segundo, teniendo tal solucion inicial toca repararla ¿cómo? con el HC y alguna mejora. Ahora bien, necesito un 
movimiento para generar un vecindario. ¿Qué movimiento? Un swap es lo común. Pero este problema me da la opción de 
hacer 2 cosas: puedo mover un AED existente a otra posición o puedo instalar uno nuevo. De acuerdo al ayudante, esto
queda a mi criterio. O sea, tengo que elegir: ¿qué hago? Ya primero hay que entender que el HC con alguna mejora es
lejos, más simple de implementar, porque tomo solo 1 decisión. NO TENGO PARA QUÉ CONSTRUIR TODO EL VECINDARIO de mis
ubicaciones.
    - Entonces, digamos que tengo la opcion de mover un AED a otra posición. Esto significaría hacer un swap entre 
    las casillas de mi array de solucion inicial. Si ese swap mejora la funcion de evaluación, entonces el array 
    modificado se vuelve mi nueva solución y sumo +1 a mi iteracion (un historial con los avances que hago)