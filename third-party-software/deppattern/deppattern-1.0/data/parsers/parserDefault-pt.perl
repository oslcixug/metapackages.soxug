#!/usr/bin/perl -X

##flag -a=analisador -c=corrector
$flag = shift(@ARGV);

## -a por defeito
if (!defined $flag) {
    $flag = "-a";
}
  
#fronteira de frase:
$Border="SENT";

#identificar nomes de dependencias lexicais (idiomaticas)
$DepLex = "^<\$|^>\$|lex\$" ;


#string com todos os atributos:
$b = "[^ _<>]*" ;

$a = "\\_<${b}>" ; ##todos os atributos
$l =  "\\_<${b}" ; ##atributos parte esquerda
$r =  "${b}>" ;  ##atributos parte direita


###########################GENERATED CODE BY COMPI#############################################

######################################## POS TAGS ########################################
$ADJ = "ADJ_[0-9]+";
$NOUN = "NOUN_[0-9]+";
$PRP = "PRP_[0-9]+";
$ADV = "ADV_[0-9]+";
$CARD = "CARD_[0-9]+";
$CONJ = "CONJ_[0-9]+";
$DT = "DT_[0-9]+";
$P = "PRO_[0-9]+";
$POS = "POS_[0-9]+";
$PCLE = "PCLE_[0-9]+";
$EX = "EX_[0-9]+";
$VERB = "VERB_[0-9]+";
$X = "[A-Z]+_[0-9]+";
$NOMINAL = "[NP][OR][UO]N?_[0-9]+";
$N = "NOUN_[0-9]+${l}type:C${r}";



#################################### LEXICAL CLASSES #####################################
$Quant  = "(?:very\|more\|less\|quite\|muy\|mucho\|bastante\|bien\|casi\|muy\|bem\|ben\|menos\|poco\|mui\|moi\|muito\|más\|mais\|pouco\|peu\|assez\|plus\|moins\|)";
$Partitive  = "(?:de\|of\|)";
$PrepLocs  = "(?:a\|de\|por\|par\|by\|to\|)";
$NincSp  = "(?:alusión\|comentario\|referencia\|llamamiento\|mención\|observación\|declaración\|propuesta\|pregunta\|)";
$NincExp  = "(?:afecto\|alegría\|amparo\|angustia\|ánimo\|cariño\|cobardía\|comprensión\|consuelo\|corte\|daño\|disgusto\|duda\|escándalo\|fobia\|gana\|gracias\|gusto\|inquietud\|miedo\|molestia\|nostalgia\|pánico\|pena\|preocupación\|recelo\|satisfacción\|sospecha\|susto\|terror\|tristeza\|vergüenza\|)";
$cliticopers = "(?:lo\|la\|los\|las\|le\|les\|nos\|os\|me\|te\|se\|)";
$PROperssuj = "(?:yo\|tú\|usted\|él\|ella\|nosotros\|vosotros\|ellos\|ellas\|ustedes\|)";
$VS  = "(?:abundar\|acabar\|aflorar\|agotar\|agradar\|anidar\|aparecer\|apasionar\|apetecer\|apuntar\|arder\|asombrar\|bastar\|bramar\|brillar\|brotar\|caber\|cesar\|circular\|comenzar\|comparecer\|concluir\|concurrir\|constar\|contrastar\|convenir\|costar\|cuajar\|cundir\|danzar\|desagradar\|desencadenar\|desfilar\|despuntar\|doler\|empezar\|encantar\|estallar\|existir\|extrañar\|faltar\|fascinar\|fastidiar\|figurar\|filtrar\|finalizar\|fracasar\|gustar\|hinchar\|holgar\|imperar\|importar\|informar\|interesar\|intervenir\|latir\|mediar\|ocurrir\|parecer\|perdurar\|predominar\|producir\|puntualizar\|quebrar\|rechinar\|reinar\|replicar\|reventar\|rugir\|sentenciar\|sobrar\|sobrevenir\|sobrevivir\|sonar\|suceder\|surgir\|temblar\|terciar\|transcurrir\|urgir\|valer\|)";
$VD  = "(?:abandonar\|abarcar\|abonar\|abordar\|abrigar\|abrir\|abrochar\|absorber\|acariciar\|acarrear\|acatar\|accionar\|acechar\|acelerar\|acentuar\|aceptar\|aclarar\|acoger\|acometer\|aconsejar\|acreditar\|acumular\|adelantar\|adivinar\|administrar\|admirar\|admitir\|adoptar\|adorar\|adquirir\|advertir\|afilar\|afirmar\|aflojar\|afrontar\|agitar\|agradecer\|agredir\|aguardar\|aguzar\|ahorrar\|alargar\|albergar\|alcanzar\|alegar\|alentar\|aligerar\|alijar\|alisar\|aliviar\|alquilar\|alterar\|alzar\|ampliar\|analizar\|añorar\|anotar\|anular\|anunciar\|apaciguar\|apagar\|aparentar\|aplacar\|aplastar\|aplazar\|aplicar\|aporrear\|aportar\|apreciar\|aprender\|apresar\|apretar\|aprovechar\|apuñalar\|argumentar\|armar\|arrancar\|arrasar\|arrastrar\|arrebatar\|arreglar\|arrogar\|articular\|asaltar\|asegurar\|asesinar\|asimilar\|aspirar\|asumir\|atacar\|atajar\|atraer\|atrapar\|atravesar\|atribuir\|aumentar\|autorizar\|aventurar\|averiguar\|avivar\|bajar\|barrer\|batir\|beber\|bendecir\|blandir\|bloquear\|bordear\|borrar\|buscar\|calar\|calcular\|calentar\|calzar\|captar\|cargar\|causar\|cazar\|celebrar\|ceñir\|cerrar\|citar\|clavar\|cobrar\|cocinar\|coger\|combinar\|cometer\|compartir\|compensar\|completar\|complicar\|componer\|comprar\|comprender\|comprobar\|comunicar\|concebir\|conceder\|condicionar\|confeccionar\|confesar\|confirmar\|conjeturar\|conocer\|conquistar\|conseguir\|conservar\|considerar\|consolidar\|constatar\|constituir\|construir\|consultar\|consumar\|contar\|contemplar\|contener\|contraer\|contratar\|controlar\|convocar\|copiar\|coronar\|corroborar\|cortar\|costar\|crear\|creer\|criticar\|cruzar\|cubrir\|culminar\|cultivar\|cumplir\|cursar\|dañar\|dar\|decidir\|declarar\|decretar\|deducir\|defender\|demandar\|demostrar\|denotar\|denunciar\|deplorar\|depositar\|derribar\|derrotar\|desabrochar\|desactivar\|desatar\|desbordar\|descalificar\|descargar\|descartar\|descentralizar\|descifrar\|descolgar\|desconocer\|descorrer\|describir\|descubrir\|desdeñar\|desear\|desechar\|desempeñar\|desenterrar\|deshacer\|designar\|desmentir\|desoír\|despachar\|despegar\|despejar\|desperdiciar\|desplegar\|despreciar\|destacar\|destapar\|desterrar\|destrozar\|destruir\|desvelar\|desviar\|detectar\|determinar\|detestar\|devolver\|devorar\|dibujar\|dictar\|difundir\|dilucidar\|dirimir\|disimular\|disipar\|disolver\|disputar\|distinguir\|divisar\|doblar\|dominar\|durar\|echar\|efectuar\|ejecutar\|ejercer\|elaborar\|elegir\|elevar\|eliminar\|elogiar\|eludir\|embarcar\|emitir\|emplear\|emprender\|empujar\|empuñar\|enarbolar\|encarecer\|encarnar\|encauzar\|encender\|encontrar\|encubrir\|enderezar\|enfilar\|enfocar\|enjugar\|enriquecer\|enseñar\|entablar\|entonar\|entorpecer\|entrañar\|entreabrir\|entregar\|entrever\|enumerar\|enviar\|erradicar\|esbozar\|escalar\|escamotear\|esclarecer\|escoger\|escribir\|escrutar\|escuchar\|esgrimir\|espantar\|esperar\|esquivar\|establecer\|estimar\|estimular\|estipular\|estirar\|estrechar\|estrenar\|estropear\|estudiar\|evaluar\|evitar\|evocar\|exaltar\|examinar\|excluir\|excusar\|exhibir\|exigir\|expeler\|experimentar\|explicar\|explorar\|expresar\|extender\|fabricar\|facilitar\|favorecer\|financiar\|fingir\|firmar\|fomentar\|forjar\|formar\|formular\|fortalecer\|forzar\|franquear\|frotar\|fruncir\|fumar\|fundar\|fundir\|ganar\|garantizar\|gastar\|generar\|gestionar\|golpear\|guardar\|habitar\|hacer\|hojear\|humedecer\|ignorar\|iluminar\|ilustrar\|imaginar\|imitar\|impartir\|impedir\|implantar\|implicar\|imponer\|imprimir\|improvisar\|impulsar\|inaugurar\|incendiar\|incluir\|incoar\|incrementar\|indagar\|indicar\|inferir\|infundir\|ingerir\|iniciar\|insinuar\|inspeccionar\|inspirar\|intentar\|intercambiar\|interpretar\|intuir\|invadir\|inventar\|invertir\|investigar\|invocar\|involucrar\|jalar\|juntar\|jurar\|justificar\|juzgar\|ladear\|lamentar\|lamer\|lanzar\|lavar\|leer\|liar\|limar\|limpiar\|liquidar\|llenar\|llevar\|localizar\|lograr\|lucir\|maldecir\|mandar\|manejar\|manifestar\|mantener\|marcar\|mascar\|masticar\|matizar\|medir\|mejorar\|mencionar\|mentar\|merecer\|minar\|modernizar\|modificar\|morder\|mostrar\|motivar\|narrar\|necesitar\|negociar\|neutralizar\|nombrar\|notar\|nublar\|observar\|obtener\|ocultar\|odiar\|ofrecer\|oír\|olvidar\|opinar\|oprimir\|ordenar\|organizar\|otear\|otorgar\|padecer\|pagar\|paladear\|palmear\|palpar\|pedir\|pegar\|pensar\|percibir\|perder\|perfeccionar\|permitir\|persuadir\|pesar\|picar\|pintar\|pisar\|pisotear\|planchar\|planear\|plantar\|plantear\|ponderar\|poner\|poseer\|postergar\|postular\|practicar\|precisar\|predecir\|preferir\|preguntar\|prender\|preparar\|presenciar\|presentir\|preservar\|presidir\|presionar\|prestar\|pretender\|pretextar\|prever\|probar\|proclamar\|procurar\|proferir\|profesar\|prohibir\|prolongar\|prometer\|promocionar\|promover\|pronunciar\|propagar\|propiciar\|propinar\|proponer\|proporcionar\|propugnar\|proseguir\|provocar\|publicar\|pulsar\|quebrantar\|quemar\|querer\|quitar\|rascar\|rasgar\|rastrear\|ratificar\|razonar\|realizar\|reanudar\|rebajar\|rebasar\|recabar\|recetar\|rechazar\|recibir\|reclamar\|recobrar\|recoger\|recomendar\|recomponer\|reconocer\|reconsiderar\|reconstruir\|recordar\|recorrer\|recortar\|recuperar\|redactar\|redoblar\|reducir\|reflejar\|reformar\|reforzar\|regalar\|registrar\|regular\|rehuir\|reiterar\|reivindicar\|relatar\|releer\|rematar\|rememorar\|remover\|renovar\|repartir\|repasar\|repetir\|reponer\|representar\|reprimir\|reprobar\|reprochar\|reproducir\|requerir\|resaltar\|rescatar\|reservar\|resolver\|respetar\|restablecer\|restar\|restaurar\|restregar\|resumir\|retomar\|retrasar\|revelar\|revisar\|revivir\|robar\|rodear\|rogar\|romper\|rozar\|rumiar\|saber\|saborear\|sacar\|sacrificar\|sacudir\|salvaguardar\|salvar\|satisfacer\|secundar\|seguir\|seleccionar\|sembrar\|señalar\|serrar\|servir\|significar\|simular\|solicitar\|solucionar\|soñar\|sopesar\|soportar\|sorber\|sortear\|sospechar\|sostener\|subrayar\|sufrir\|sugerir\|sujetar\|suministrar\|superar\|suplantar\|suponer\|suprimir\|suscitar\|suspender\|sustentar\|sustituir\|tapar\|tararear\|temer\|tener\|tentar\|terminar\|tocar\|tolerar\|tomar\|torcer\|traer\|tragar\|transmitir\|transportar\|traspasar\|trasponer\|trazar\|usar\|utilizar\|vaciar\|valer\|valorar\|velar\|vencer\|vender\|ver\|verificar\|visitar\|vislumbrar\|voltear\|vomitar\|zampar\|zanjar\|)";
$IV  = "(?:agradar\|)";
$VI  = "(?:adeudar\|arengar\|hurtar\|)";
$SV  = "(?:absorber\|acarrear\|acatar\|acentuar\|aconsejar\|adeudar\|adquirir\|afectar\|afeitar\|agolpar\|agredir\|alterar\|amanecer\|amortiguar\|angustiar\|antojar\|anunciar\|apremiar\|ascender\|asentar\|asentir\|asumir\|autorizar\|barrer\|calificar\|carecer\|causar\|ceder\|cegar\|cobrar\|coincidir\|competir\|comprometer\|concordar\|condicionar\|conducir\|confirmar\|conmover\|consistir\|constituir\|corresponder\|corroborar\|crecer\|criticar\|culminar\|dañar\|deber\|decaer\|declarar\|declinar\|decrecer\|decretar\|delatar\|denotar\|derrotar\|desaparecer\|desarrollar\|desconcertar\|designar\|desmentir\|despejar\|destacar\|destruir\|desvanecer\|deteriorar\|dictar\|diferir\|diluir\|discurrir\|disminuir\|dispersar\|disputar\|dividir\|durar\|empeorar\|encargar\|enojar\|entrañar\|entusiasmar\|envejecer\|equivaler\|erguir\|esbozar\|escurrir\|estremecer\|evolucionar\|extinguir\|fallar\|fallecer\|florecer\|fruncir\|funcionar\|generar\|iluminar\|impacientar\|impedir\|implicar\|impresionar\|inaugurar\|incidir\|inclinar\|incluir\|indicar\|inducir\|infiltrar\|influir\|inquietar\|inspirar\|invadir\|involucrar\|irritar\|irrumpir\|manifestar\|materializar\|merecer\|multiplicar\|nublar\|obedecer\|opinar\|oscilar\|otorgar\|persistir\|pertenecer\|pesar\|preceder\|presidir\|propiciar\|provenir\|provocar\|ratificar\|reaparecer\|rebasar\|rebelar\|reclinar\|reflejar\|regular\|reiterar\|remontar\|rendir\|repercutir\|replegar\|requerir\|resbalar\|resonar\|restar\|resucitar\|resultar\|resumir\|revelar\|rodar\|secundar\|ser\|significar\|sobresaltar\|sonar\|sorprender\|subrayar\|sufrir\|sugerir\|suplantar\|suscitar\|sustentar\|tender\|tornar\|transformar\|traspasar\|trastornar\|variar\|vibrar\|yacer\|)";
$VRa  = "(?:acceder\|acercar\|acertar\|acostumbrar\|acudir\|adaptar\|afectar\|aferrar\|aficionar\|afiliar\|ajustar\|aludir\|apelar\|apostar\|apresurar\|aproximar\|arrimar\|arrojar\|asistir\|asomar\|atener\|atinar\|atrever\|avenir\|comprometer\|conducir\|contribuir\|corresponder\|deber\|dedicar\|dirigir\|disponer\|enfrentar\|equivaler\|exponer\|inducir\|invitar\|ir\|limitar\|llegar\|mudar\|negar\|obligar\|oler\|oponer\|pertenecer\|precipitar\|proceder\|recurrir\|referir\|regresar\|remontar\|renunciar\|resignar\|resistir\|someter\|subir\|sumar\|sustraer\|tender\|trasladar\|unir\|vincular\|volver\|)";
$VRde  = "(?:abstener\|abusar\|acordar\|acusar\|adueñar\|aislar\|alegrar\|apartar\|apiadar\|apoderar\|apropiar\|avergonzar\|burlar\|carecer\|cerciorar\|convencer\|depender\|derivar\|desconfiar\|desembarazar\|desentender\|despojar\|desprender\|diferenciar\|diferir\|disfrazar\|disfrutar\|dudar\|enamorar\|encargar\|encoger\|enterar\|extraer\|fiar\|gozar\|hablar\|huir\|liberar\|librar\|ocupar\|partir\|percatar\|prescindir\|presumir\|privar\|provenir\|salir\|separar\|tratar\|vestir\|)";
$VRen  = "(?:acodar\|adentrar\|ahondar\|apoyar\|asentar\|basar\|caer\|centrar\|colar\|complacer\|concentrar\|confiar\|consistir\|convertir\|desembocar\|dividir\|empecinar\|empeñar\|encarnar\|encerrar\|entrar\|envolver\|esconder\|esforzar\|especializar\|fijar\|flotar\|hundir\|hurgar\|incidir\|incurrir\|infiltrar\|influir\|ingresar\|inscribir\|insistir\|instalar\|internar\|introducir\|irrumpir\|materializar\|matricular\|meter\|nacer\|obstinar\|oscilar\|participar\|penetrar\|plantificar\|profundizar\|prorrumpir\|radicar\|recostar\|recrear\|refugiar\|reparar\|repercutir\|residir\|sentar\|situar\|soñar\|sumergir\|sumir\|tardar\|transformar\|tumbar\|veranear\|zambullir\|)";
$VRcon  = "(?:asociar\|atentar\|bastar\|coincidir\|colaborar\|comparar\|conectar\|conformar\|confundir\|conversar\|convivir\|encarar\|enlazar\|entrevistar\|identificar\|mezclar\|rebelar\|reconciliar\|relacionar\|soñar\|topar\|tropezar\|)";
$VRpor  = "(?:abogar\|brindar\|caracterizar\|deambular\|luchar\|optar\|pugnar\|vagar\|velar\|)";
$VRhacia  = "(?:encaminar\|)";
$VRsobre  = "(?:abalanzar\|)";
$Vintr  = "(?:abrazar\|aburrir\|acomodar\|acompañar\|acostar\|actuar\|acunar\|acurrucar\|agachar\|agregar\|aguantar\|ahogar\|alarmar\|alejar\|alimentar\|almorzar\|amanecer\|amar\|amenazar\|añadir\|andar\|animar\|anochecer\|aparcar\|apear\|aplaudir\|aprobar\|apurar\|arañar\|arrepentir\|arrodillar\|arrugar\|ascender\|asustar\|atar\|atender\|atisbar\|atormentar\|aullar\|auscultar\|ausentar\|avanzar\|ayudar\|bailar\|bañar\|beber\|besar\|bombardear\|bordar\|bromear\|cagar\|callar\|calmar\|caminar\|cansar\|cantar\|canturrear\|casar\|castigar\|cavilar\|cenar\|charlar\|chillar\|colgar\|combatir\|comentar\|comer\|compadecer\|condenar\|congelar\|consentir\|consolar\|contagiar\|contestar\|continuar\|contradecir\|corregir\|correr\|coser\|criar\|cuidar\|cundir\|curar\|decepcionar\|decir\|definir\|defraudar\|degollar\|demorar\|derrumbar\|desafiar\|desangrar\|desanimar\|desayunar\|descansar\|descuidar\|desengañar\|desesperar\|desistir\|deslizar\|deslumbrar\|desmayar\|desnudar\|despedir\|despertar\|desplomar\|desvariar\|desvivir\|detener\|disculpar\|discutir\|disparar\|distraer\|divertir\|dormir\|drogar\|educar\|emborrachar\|embromar\|emerger\|emocionar\|enfadar\|enfermar\|enfriar\|enfurecer\|engañar\|engordar\|enloquecer\|enrollar\|ensayar\|ensuciar\|entender\|enterrar\|entrenar\|entretener\|envidiar\|equivocar\|escapar\|escupir\|espabilar\|espiar\|estorbar\|estornudar\|estrangular\|estrellar\|exagerar\|excitar\|exclamar\|expulsar\|extraviar\|felicitar\|follar\|frenar\|gemir\|gesticular\|girar\|grabar\|gritar\|gruñir\|guiar\|herir\|incapacitar\|incorporar\|indignar\|inmovilizar\|insultar\|interrogar\|interrumpir\|jadear\|joder\|jugar\|ladrar\|largar\|levantar\|llamar\|llorar\|llover\|madrugar\|manipular\|marchar\|marear\|mascullar\|masturbar\|matar\|mear\|menear\|mentir\|mirar\|molestar\|moquear\|morir\|mosquear\|mover\|murmurar\|nadar\|nevar\|ofender\|olfatear\|operar\|orientar\|orinar\|oscurecer\|parar\|parir\|parpadear\|pasear\|patalear\|peinar\|pelar\|pelear\|perdonar\|perpetuar\|perseguir\|pescar\|pillar\|pinchar\|pitar\|poder\|preocupar\|presentar\|prevenir\|proteger\|protestar\|pudrir\|quejar\|rajar\|reaccionar\|reanimar\|rebuscar\|rebuznar\|recapacitar\|recargar\|recitar\|recluir\|recomenzar\|rectificar\|reflexionar\|reír\|relucir\|remediar\|reñir\|reposar\|reprender\|repugnar\|resoplar\|respirar\|responder\|retirar\|retroceder\|retumbar\|reunir\|revolotear\|revolver\|rezar\|saltar\|saludar\|sanar\|secar\|seguir\|silbar\|sobreponer\|socorrer\|sollozar\|soltar\|sonreír\|soplar\|sudar\|suplicar\|suspirar\|susurrar\|telefonear\|torturar\|toser\|trabajar\|traducir\|traicionar\|tranquilizar\|triunfar\|tutear\|vacilar\|vengar\|venir\|viajar\|vigilar\|violar\|volar\|zarandear\|)";
$Vatr  = "(?:acabar\|actuar\|amanecer\|andar\|antojar\|aparecer\|caer\|considerar\|continuar\|costar\|creer\|dar\|declarar\|definir\|dejar\|ejercer\|encontrar\|erigir\|estar\|hacer\|hallar\|imaginar\|ingresar\|ir\|llamar\|llevar\|manifestar\|mantener\|meter\|mostrar\|nacer\|parecer\|pasar\|permanecer\|poner\|proseguir\|quedar\|resultar\|revelar\|saber\|salir\|seguir\|sentir\|ser\|sonar\|tener\|terminar\|tirar\|titular\|tornar\|trabajar\|transcurrir\|venir\|ver\|vivir\|volver\|yacer\|)";
$Vtr  = "(?:abandonar\|abarcar\|ablandar\|abolir\|abonar\|abordar\|aborrecer\|abrasar\|abrazar\|abreviar\|abrigar\|abrir\|abrochar\|abrumar\|absorber\|acabar\|acallar\|acaparar\|acariciar\|acarrear\|acatar\|accionar\|acechar\|acelerar\|acentuar\|aceptar\|acercar\|acertar\|aclarar\|acoger\|acometer\|acomodar\|acompañar\|aconsejar\|acordar\|acortar\|acostar\|acrecentar\|acreditar\|acumular\|acusar\|adaptar\|adelantar\|adeudar\|adivinar\|adjudicar\|administrar\|admirar\|admitir\|adoptar\|adorar\|adornar\|adquirir\|aducir\|advertir\|afear\|aferrar\|afianzar\|afilar\|afinar\|afirmar\|afligir\|aflojar\|afrontar\|agarrar\|agitar\|agotar\|agradecer\|agravar\|agredir\|agregar\|agrupar\|aguantar\|aguardar\|aguzar\|ahogar\|ahondar\|ahorrar\|ahuyentar\|aislar\|ajustar\|alabar\|alargar\|alarmar\|albergar\|alcanzar\|alegar\|alegrar\|alejar\|alentar\|aligerar\|alijar\|alimentar\|alinear\|alisar\|aliviar\|alquilar\|alterar\|alumbrar\|alzar\|amagar\|amar\|amargar\|amenazar\|amenizar\|ampliar\|añadir\|analizar\|anhelar\|animar\|añorar\|anotar\|ansiar\|anticipar\|anular\|anunciar\|apaciguar\|apagar\|aparcar\|aparejar\|aparentar\|apartar\|aplacar\|aplastar\|aplaudir\|aplazar\|aplicar\|aporrear\|aportar\|apostar\|apoyar\|apreciar\|aprehender\|aprender\|apresar\|apretar\|aprobar\|aprovechar\|apuñalar\|apuntar\|apurar\|arañar\|arbitrar\|argumentar\|armar\|arrancar\|arrasar\|arrastrar\|arrear\|arrebatar\|arreglar\|arriesgar\|arrimar\|arrogar\|arrojar\|arrostrar\|articular\|asaltar\|asegurar\|asesinar\|asestar\|asignar\|asimilar\|asistir\|asociar\|asomar\|aspirar\|asumir\|asustar\|atacar\|atajar\|atenazar\|atender\|atenuar\|atraer\|atrapar\|atravesar\|atribuir\|augurar\|aumentar\|autorizar\|avanzar\|aventurar\|averiguar\|avisar\|avivar\|ayudar\|azotar\|bailar\|bajar\|balbucear\|bañar\|barrer\|batir\|bautizar\|beber\|bendecir\|beneficiar\|besar\|blandir\|bloquear\|bombardear\|bordear\|borrar\|brindar\|burlar\|buscar\|calar\|calcular\|calentar\|calificar\|callar\|calmar\|calzar\|cambiar\|caminar\|cantar\|canturrear\|captar\|capturar\|caracterizar\|cargar\|castigar\|catalogar\|causar\|cavilar\|cazar\|ceder\|celebrar\|ceñir\|censurar\|centrar\|cerrar\|chupar\|cifrar\|citar\|clasificar\|clausurar\|clavar\|cobrar\|cocinar\|codificar\|coger\|colgar\|colmar\|colocar\|combinar\|comentar\|comenzar\|comer\|comercializar\|cometer\|compaginar\|comparar\|compartir\|compensar\|complacer\|completar\|complicar\|componer\|comprar\|comprender\|comprobar\|comprometer\|comunicar\|concebir\|conceder\|concentrar\|concertar\|conciliar\|concitar\|concluir\|concretar\|condenar\|condicionar\|conducir\|conectar\|confeccionar\|conferir\|confesar\|confiar\|configurar\|confirmar\|confiscar\|conformar\|confundir\|conjeturar\|conjurar\|conmover\|conocer\|conquistar\|consagrar\|conseguir\|consentir\|conservar\|considerar\|consolidar\|constatar\|constituir\|construir\|consultar\|consumar\|consumir\|contabilizar\|contagiar\|contar\|contemplar\|contener\|contestar\|continuar\|contradecir\|contraer\|contratar\|controlar\|convencer\|convertir\|convocar\|coordinar\|copiar\|coronar\|corregir\|correr\|corroborar\|cortar\|cosechar\|coser\|costar\|costear\|crear\|crecer\|creer\|criar\|criticar\|cruzar\|cubrir\|cuestionar\|cuidar\|culminar\|cultivar\|cumplir\|curar\|cursar\|dañar\|dar\|deber\|decepcionar\|decidir\|decir\|declarar\|decretar\|dedicar\|deducir\|defender\|definir\|defraudar\|degradar\|dejar\|delatar\|demandar\|demorar\|demostrar\|denominar\|denotar\|denunciar\|deparar\|deplorar\|deponer\|depositar\|derramar\|derribar\|derrotar\|desabrochar\|desactivar\|desafiar\|desalojar\|desarrollar\|desatar\|desatender\|desayunar\|desbaratar\|desbordar\|descabezar\|descalificar\|descargar\|descartar\|descentralizar\|descifrar\|descolgar\|descomponer\|desconcertar\|desconocer\|descorrer\|describir\|descubrir\|descuidar\|desdeñar\|desear\|desechar\|desembarcar\|desempeñar\|desencadenar\|desenterrar\|desestimar\|desgarrar\|desgastar\|desgranar\|deshacer\|designar\|deslizar\|desmentir\|desnudar\|desoír\|despachar\|despedir\|despegar\|despejar\|desperdiciar\|despertar\|desplazar\|desplegar\|despojar\|despreciar\|desprender\|destacar\|destapar\|desterrar\|destinar\|destrozar\|destruir\|desvelar\|desviar\|detallar\|detectar\|detener\|determinar\|detestar\|devolver\|devorar\|dibujar\|dictar\|dificultar\|difuminar\|difundir\|dignar\|dilatar\|dilucidar\|dirigir\|dirimir\|disculpar\|discutir\|diseñar\|disfrazar\|disfrutar\|disimular\|disipar\|disminuir\|disolver\|disparar\|dispensar\|dispersar\|disponer\|disputar\|distar\|distinguir\|distraer\|distribuir\|divertir\|dividir\|divisar\|doblar\|dominar\|dormir\|dramatizar\|dudar\|duplicar\|durar\|echar\|editar\|educar\|efectuar\|ejecutar\|ejercer\|elaborar\|elegir\|elevar\|eliminar\|elogiar\|eludir\|emanar\|embarcar\|emitir\|empañar\|empapar\|empezar\|emplear\|emprender\|empujar\|empuñar\|enarbolar\|enardecer\|encabezar\|encajar\|encarar\|encarecer\|encargar\|encarnar\|encauzar\|encender\|encerrar\|enchufar\|encoger\|encomendar\|encomiar\|encontrar\|encubrir\|enderezar\|enfilar\|enfocar\|enfrentar\|engañar\|engendrar\|enjugar\|enjuiciar\|enlazar\|enmarcar\|enriquecer\|ensalzar\|ensayar\|enseñar\|ensuciar\|entablar\|entender\|enterrar\|entibiar\|entonar\|entorpecer\|entrañar\|entreabrir\|entrecerrar\|entregar\|entretener\|entrever\|entrevistar\|entusiasmar\|enumerar\|enunciar\|enviar\|envidiar\|envolver\|erguir\|erradicar\|esbozar\|escalar\|escamotear\|esclarecer\|escoger\|esconder\|escribir\|escrutar\|escuchar\|escupir\|esgrimir\|espantar\|especificar\|esperar\|espiar\|esquivar\|establecer\|estimar\|estimular\|estipular\|estirar\|estorbar\|estrechar\|estrellar\|estremecer\|estrenar\|estropear\|estudiar\|evacuar\|evaluar\|evitar\|evocar\|exaltar\|examinar\|excitar\|excluir\|excusar\|exhalar\|exhibir\|exhortar\|exigir\|expeler\|experimentar\|explicar\|explorar\|explotar\|exponer\|expresar\|expulsar\|extender\|extinguir\|extraer\|extrañar\|fabricar\|facilitar\|fallar\|fascinar\|favorecer\|felicitar\|figurar\|fijar\|financiar\|fingir\|firmar\|fomentar\|forjar\|formar\|formular\|fortalecer\|fortificar\|forzar\|franquear\|frecuentar\|frenar\|frotar\|fruncir\|frustrar\|fumar\|fundamentar\|fundar\|fundir\|ganar\|garantizar\|gastar\|generar\|gestionar\|girar\|gobernar\|golpear\|gozar\|grabar\|gritar\|guardar\|guiar\|gustar\|haber\|habilitar\|habitar\|hablar\|hacer\|hallar\|heredar\|herir\|hincar\|hinchar\|hojear\|honrar\|humedecer\|humillar\|hundir\|identificar\|ignorar\|iluminar\|ilustrar\|imaginar\|imitar\|impartir\|impedir\|implantar\|implicar\|imponer\|impregnar\|impresionar\|imprimir\|improvisar\|impulsar\|inaugurar\|incendiar\|incitar\|inclinar\|incluir\|incoar\|incorporar\|incrementar\|inculcar\|incumplir\|indagar\|indicar\|inducir\|inferir\|informar\|infundir\|ingerir\|iniciar\|inmovilizar\|insertar\|insinuar\|inspeccionar\|inspirar\|instalar\|instar\|insultar\|integrar\|intensificar\|intentar\|intercalar\|intercambiar\|interponer\|interpretar\|interrogar\|interrumpir\|intimidar\|introducir\|intuir\|inundar\|invadir\|invalidar\|inventar\|invertir\|investigar\|invitar\|invocar\|involucrar\|inyectar\|irradiar\|irritar\|jalar\|joder\|jugar\|juntar\|jurar\|justificar\|juzgar\|labrar\|ladear\|lamentar\|lamer\|lanzar\|largar\|lavar\|leer\|legalizar\|legitimar\|levantar\|liar\|liberar\|librar\|limar\|limitar\|limpiar\|liquidar\|llamar\|llenar\|llevar\|llorar\|localizar\|lograr\|lubricar\|lucir\|lucubrar\|maldecir\|malgastar\|maltratar\|manchar\|mandar\|manejar\|manifestar\|mantener\|marcar\|mascar\|masticar\|matar\|matizar\|medir\|mejorar\|memorizar\|mencionar\|menoscabar\|mentar\|merecer\|meter\|mezclar\|mimar\|minar\|mirar\|modelar\|modernizar\|modificar\|mojar\|montar\|morder\|mostrar\|motivar\|mover\|movilizar\|multiplicar\|murmurar\|narrar\|necesitar\|negar\|negociar\|neutralizar\|nombrar\|notar\|notificar\|nublar\|nutrir\|obedecer\|objetar\|obligar\|observar\|obstaculizar\|obtener\|ocasionar\|ocultar\|ocupar\|odiar\|ofender\|ofertar\|ofrecer\|ofrendar\|oír\|oler\|olvidar\|omitir\|opinar\|oponer\|oprimir\|ordenar\|organizar\|orientar\|originar\|ostentar\|otear\|otorgar\|padecer\|pagar\|paladear\|palmear\|palpar\|paralizar\|parar\|parodiar\|partir\|pasar\|pasear\|pedir\|pegar\|pensar\|percibir\|perder\|perdonar\|perfeccionar\|perforar\|perjudicar\|permitir\|perpetrar\|perpetuar\|perseguir\|persuadir\|perturbar\|pesar\|pescar\|picar\|pillar\|pinchar\|pintar\|pisar\|pisotear\|planchar\|planear\|plantar\|plantear\|plasmar\|poblar\|poder\|ponderar\|poner\|portar\|posar\|poseer\|postergar\|postular\|potenciar\|practicar\|preceder\|precipitar\|precisar\|predecir\|predicar\|preferir\|preguntar\|prender\|preocupar\|preparar\|presagiar\|prescribir\|presenciar\|presentar\|presentir\|preservar\|presidir\|presionar\|prestar\|presumir\|presuponer\|pretender\|pretextar\|prevenir\|prever\|privar\|probar\|proclamar\|procurar\|prodigar\|producir\|proferir\|profesar\|programar\|prohibir\|prolongar\|prometer\|promocionar\|promover\|pronunciar\|propagar\|propiciar\|propinar\|proponer\|proporcionar\|propugnar\|proseguir\|protagonizar\|proteger\|provocar\|proyectar\|publicar\|pulsar\|pulverizar\|puntualizar\|quebrantar\|quebrar\|quemar\|querer\|quitar\|rajar\|rascar\|rasgar\|rastrear\|ratificar\|razonar\|realizar\|realzar\|reanudar\|reavivar\|rebajar\|rebasar\|rebatir\|recabar\|recetar\|rechazar\|recibir\|recitar\|reclamar\|recobrar\|recoger\|recomendar\|recomponer\|reconocer\|reconsiderar\|reconstruir\|recordar\|recorrer\|recortar\|rectificar\|recuperar\|recurrir\|redactar\|redoblar\|reducir\|reemplazar\|referir\|reflejar\|reflexionar\|reformar\|reforzar\|refrescar\|refutar\|regalar\|regar\|regir\|registrar\|regular\|rehacer\|rehuir\|rehusar\|reír\|reiterar\|reivindicar\|relacionar\|relatar\|releer\|relegar\|rellenar\|rematar\|remedar\|remediar\|rememorar\|remitir\|remover\|rendir\|renovar\|repartir\|repasar\|repetir\|replicar\|reponer\|reportar\|representar\|reprimir\|reprobar\|reprochar\|reproducir\|requerir\|resaltar\|rescatar\|rescindir\|reservar\|resistir\|resolver\|respaldar\|respetar\|respirar\|responder\|restablecer\|restañar\|restar\|restaurar\|restregar\|resucitar\|resumir\|retener\|retirar\|retomar\|retrasar\|retratar\|retroceder\|reunir\|revelar\|reventar\|revisar\|revivir\|revolver\|rezar\|robar\|rociar\|rodar\|rodear\|rogar\|romper\|rondar\|rozar\|rumiar\|saber\|saborear\|sacar\|sacrificar\|sacudir\|salpicar\|saltar\|saludar\|salvaguardar\|salvar\|sancionar\|satisfacer\|secar\|secundar\|segar\|seguir\|seleccionar\|sellar\|sembrar\|señalar\|sentar\|sentenciar\|sentir\|separar\|serrar\|servir\|significar\|simbolizar\|simular\|sistematizar\|situar\|sobrepasar\|sobrevolar\|socorrer\|sofocar\|solicitar\|soltar\|solucionar\|someter\|soñar\|sonsacar\|sopesar\|soplar\|soportar\|sorber\|sorprender\|sortear\|sospechar\|sostener\|suavizar\|subir\|subrayar\|substituir\|subsumir\|suceder\|sufrir\|sugerir\|sujetar\|sumar\|suministrar\|superar\|suplantar\|suplicar\|suplir\|suponer\|suprimir\|suscitar\|suspender\|sustentar\|sustituir\|sustraer\|susurrar\|tachar\|tapar\|tararear\|tardar\|tejer\|telefonear\|temer\|tender\|tener\|teñir\|tentar\|terminar\|tirar\|tocar\|tolerar\|tomar\|torcer\|totalizar\|trabajar\|traducir\|traer\|tragar\|traicionar\|tranquilizar\|transcribir\|transformar\|transmitir\|transportar\|trascender\|trasladar\|trasmitir\|traspasar\|trasponer\|trastornar\|tratar\|trazar\|trenzar\|trocar\|turbar\|ubicar\|unir\|urdir\|urgir\|usar\|usurpar\|utilizar\|vaciar\|valer\|valorar\|velar\|vencer\|vender\|vengar\|ver\|verificar\|verter\|vestir\|viciar\|vigilar\|violar\|visitar\|vislumbrar\|vivir\|volcar\|voltear\|volver\|vomitar\|zampar\|zanjar\|)";


####################################END CODE BY COMPI################################################



$i=0;
$listTags="";
$seq="";
$CountLines=0;

$j=0;
while (<>) {
   chop($_);

  ($token, $info) = split(" ", $_);

  if ( ($CountLines % 100) == 0) {;
       printf  STDERR "- - - processar linha:(%6d) - - -\r",$CountLines;
  }
  $CountLines++;


  ##organizamos a informacao de cada token:
   if ($info ne "") {
     (@info) = split ('\|', $info);
     for ($i=0;$i<=$#info;$i++) {
       if ($info[$i] ne "") {
        ($attrib, $value) = split (':', $info[$i]) ;
        $Exp{$attrib} = $value ;
       }
     }
   }

   ##construimos os vectores da oracao
   if ($Exp{"tag"} ne $Border) {
     $Token[$j] = $token ;
     if ($info ne "") {
       $Lemma[$j] = $Exp{"lemma"} ;
       $Tag[$j] = $Exp{"tag"} ;
       $Attributes[$j] = "";
       foreach $at (sort keys %Exp) {
	 if ($at ne "tag"){
	     $Attributes[$j] .= "$at:$Exp{$at}|" ; 
             $ATTR[$j]{$at} = $Exp{$at} ;
         }
       }
       foreach $at (sort keys %Exp) {
	   delete $Exp{$at};
       }
     }

     $j++;
     #print STDERR "$j\r";
   }

   elsif ($Exp{"tag"} eq $Border) {
     $Token[$j] = $token ;
     $Lemma[$j] = $Exp{"lemma"} ;
     $Tag[$j] = $Exp{"tag"} ;
     $Attributes[$j] = "";
     foreach $at (sort keys %Exp) {
         if ($at ne "tag"){
             $Attributes[$j] .= "$at:$Exp{$at}|" ;
             $ATTR[$j]{$at} = $Exp{$at} ;
         }
     }
     foreach $at (sort keys %Exp) {
           delete $Exp{$at};
     }


     ##construimos os strings com a lista de tags-atributos e os token-tags da oraÃ§ao
     for ($i=0;$i<=$#Token;$i++) {

       $listTags .= "$Tag[$i]_${i}_<$Attributes[$i]>" ;
       $seq .= "$Token[$i]_$Tag[$i]_${i}_<$Attributes[$i]>" . " ";

	 
      }##fim do for
     # $seq .= "\." . "_" . $Border ;
     
      $STOP=0;
      #$iteration=0;
      while (!$STOP) {
         $ListInit = $listTags;
         #$iteration++;
        # print STDERR "$iteration\n";


###########################BEGIN GRAMMAR#############################################
# AdjnL: ADV<lemma:$Quant> ADJ|ADV
# Recursivity: 1
# Agreement:
# Inherit:
# Add:
(@temp) = ($listTags =~ /($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/$2/g;
(@temp) = ($listTags =~ /($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/$2/g;

# SpecL: P<token:[Ss]e> VERB
(@temp) = ($listTags =~ /($P${l}token:[Ss]e\|${r})($VERB$a)/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($P${l}token:[Ss]e\|${r})($VERB$a)/$2/g;

# SpecR: VERB P<token:[Ss]e>
(@temp) = ($listTags =~ /($VERB$a)($P${l}token:[Ss]e\|${r})/g);
$Rel =  "SpecR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($P${l}token:[Ss]e\|${r})/$1/g;

# AdjnL: [VERB] ADV VERB
(@temp) = ($listTags =~ /(?:$VERB$a)($ADV$a)($VERB$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($ADV$a)($VERB$a)/$1$3/g;

# AdjnL: ADV VERB
# Recursivity: 1
(@temp) = ($listTags =~ /($ADV$a)($VERB$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV$a)($VERB$a)/$2/g;
(@temp) = ($listTags =~ /($ADV$a)($VERB$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV$a)($VERB$a)/$2/g;

# AdjnR: VERB ADV
# Recursivity: 1
(@temp) = ($listTags =~ /($VERB$a)($ADV$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($ADV$a)/$1/g;
(@temp) = ($listTags =~ /($VERB$a)($ADV$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($ADV$a)/$1/g;

# SpecL: VERB<type:S> VERB<mode:P>
# Inherit: tense, person, number
# Add: voice:passive
(@temp) = ($listTags =~ /($VERB${l}type:S\|${r})($VERB${l}mode:P\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB${l}type:S\|${r})($VERB${l}mode:P\|${r})/$2/g;
Inherit("DepHead","tense,person,number",@temp);
Add("DepHead","voice:passive",@temp);

# SpecL: VERB<type:A> VERB<mode:P>
# Inherit: tense, mode, person, number
# Add: type:perfect
(@temp) = ($listTags =~ /($VERB${l}type:A\|${r})($VERB${l}mode:P\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB${l}type:A\|${r})($VERB${l}mode:P\|${r})/$2/g;
Inherit("DepHead","tense,mode,person,number",@temp);
Add("DepHead","type:perfect",@temp);

# SpecL: VERB VERB<mode:N>
# Recursivity: 1
# Inherit: mode, person, tense, number
(@temp) = ($listTags =~ /($VERB$a)($VERB${l}mode:N\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($VERB${l}mode:N\|${r})/$2/g;
Inherit("DepHead","mode,person,tense,number",@temp);
(@temp) = ($listTags =~ /($VERB$a)($VERB${l}mode:N\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($VERB${l}mode:N\|${r})/$2/g;
Inherit("DepHead","mode,person,tense,number",@temp);

# CprepL: VERB PRP<lemma:$PrepLocs>|CONJ<lemma:que&type:S> VERB<mode:N>
# Recursivity: 1
# Inherit: mode, person, tense, number
(@temp) = ($listTags =~ /($VERB$a)($PRP${l}lemma:$PrepLocs\|${r}|$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/g);
$Rel =  "CprepL";
DepRelHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($PRP${l}lemma:$PrepLocs\|${r}|$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/$3/g;
Inherit("DepRelHead","mode,person,tense,number",@temp);
(@temp) = ($listTags =~ /($VERB$a)($PRP${l}lemma:$PrepLocs\|${r}|$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/g);
$Rel =  "CprepL";
DepRelHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($PRP${l}lemma:$PrepLocs\|${r}|$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/$3/g;
Inherit("DepRelHead","mode,person,tense,number",@temp);

# AdjnL: ADJ|CARD NOUN
# Recursivity: 1
# Agr: number, gender
(@temp) = ($listTags =~ /($ADJ$a|$CARD$a)($NOUN$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"number,gender",@temp);
$listTags =~ s/($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;
(@temp) = ($listTags =~ /($ADJ$a|$CARD$a)($NOUN$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"number,gender",@temp);
$listTags =~ s/($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;

# AdjnR: NOUN ADJ|CARD
# Recursivity: 1
# Agr: number, gender
(@temp) = ($listTags =~ /($NOUN$a)($ADJ$a|$CARD$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"number,gender",@temp);
$listTags =~ s/($NOUN${l}concord:1${r})($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})/$1/g;
$listTags =~ s/concord:[01]\|//g;
(@temp) = ($listTags =~ /($NOUN$a)($ADJ$a|$CARD$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"number,gender",@temp);
$listTags =~ s/($NOUN${l}concord:1${r})($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})/$1/g;
$listTags =~ s/concord:[01]\|//g;

# AdjnR: NOUN NOUN
# Recursivity: 1
(@temp) = ($listTags =~ /($NOUN$a)($NOUN$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a)($NOUN$a)/$1/g;
(@temp) = ($listTags =~ /($NOUN$a)($NOUN$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a)($NOUN$a)/$1/g;

# SpecL: DT NOUN
# Recursivity: 1
# Agr: number, gender
(@temp) = ($listTags =~ /($DT$a)($NOUN$a)/g);
$Rel =  "SpecL";
DepHead($Rel,"number,gender",@temp);
$listTags =~ s/($DT${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;
(@temp) = ($listTags =~ /($DT$a)($NOUN$a)/g);
$Rel =  "SpecL";
DepHead($Rel,"number,gender",@temp);
$listTags =~ s/($DT${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;

# CprepR: [NOUN] [PRP<lemma:$Partitive>] NOUN PRP<lemma:$Partitive> NOUN
(@temp) = ($listTags =~ /(?:$NOUN$a)(?:$PRP${l}lemma:$Partitive\|${r})($NOUN$a)($PRP${l}lemma:$Partitive\|${r})($NOUN$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a)($PRP${l}lemma:$Partitive\|${r})($NOUN$a)($PRP${l}lemma:$Partitive\|${r})($NOUN$a)/$1$2$3/g;

# CprepR: NOUN|P PRP NOUN|P
# Recursivity: 2
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/$1/g;
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/$1/g;
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/$1/g;

# SubjL: [NOUN|P] P<type:R> VERB [NOUN|P]?
# NEXT
# DobjR: [NOUN|P] [P<type:R>] VERB NOUN|P
# NEXT
# SubjL<Relative>: NOUN|P [P<type:R>] VERB [NOUN|P]?
# Agr: number, person
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)($P${l}type:R\|${r})($VERB$a)(?:$NOUN$a|$P$a)?/g);
$Rel =  "SubjL";
DepHead($Rel,"",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)(?:$P${l}type:R\|${r})($VERB$a)($NOUN$a|$P$a)/g);
$Rel =  "DobjR";
HeadDep($Rel,"",@temp);
(@temp) = ($listTags =~ /($NOUN$a|$P$a)(?:$P${l}type:R\|${r})($VERB$a)(?:$NOUN$a|$P$a)?/g);
$Rel =  "SubjL<Relative>";
HeadDep($Rel,"number,person",@temp);
$listTags =~ s/($NOUN${l}concord:1${r}|$P${l}concord:1${r})($P${l}type:R\|${r})($VERB${l}concord:1${r})($NOUN$a|$P$a)?/$1/g;
$listTags =~ s/concord:[01]\|//g;

# DobjL: [NOUN|P] P<type:R> [NOUN] VERB
# NEXT
# SubjL: [NOUN|P] [P<type:R>] NOUN VERB
# Agr: number, person
# NEXT
# DobjL<Relative>: NOUN|P [P<type:R>] [NOUN] VERB
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)($P${l}type:R\|${r})(?:$NOUN$a)($VERB$a)/g);
$Rel =  "DobjL";
DepHead($Rel,"",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)(?:$P${l}type:R\|${r})($NOUN$a)($VERB$a)/g);
$Rel =  "SubjL";
DepHead($Rel,"number,person",@temp);
(@temp) = ($listTags =~ /($NOUN$a|$P$a)(?:$P${l}type:R\|${r})(?:$NOUN$a)($VERB$a)/g);
$Rel =  "DobjL<Relative>";
HeadDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($P${l}type:R\|${r})($NOUN${l}concord:1${r})($VERB${l}concord:1${r})/$1/g;

# DobjL: NOUN|P VERB<mode:P>
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($VERB${l}mode:P\|${r})/g);
$Rel =  "DobjL";
DepHead($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($VERB${l}mode:P\|${r})/$2/g;

# AdjnR<Attribute>: NOUN|P [VERB] ADJ [PRP]? [NOUN|P]?
# Agr:gender,number
# NEXT
# CprepR: [NOUN|P] [VERB] ADJ PRP NOUN|P
# NEXT
# AdjnR: [NOUN|P] VERB ADJ [PRP]? [NOUN|P]?
# NEXT
# SubjL: NOUN|P VERB [ADJ] [PRP]? [NOUN|P]?
# Agr: number, person
(@temp) = ($listTags =~ /($NOUN$a|$P$a)(?:$VERB$a)($ADJ$a)(?:$PRP$a)?(?:$NOUN$a|$P$a)?/g);
$Rel =  "AdjnR<Attribute>";
HeadDep($Rel,"gender,number",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)(?:$VERB$a)($ADJ$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)($VERB$a)($ADJ$a)(?:$PRP$a)?(?:$NOUN$a|$P$a)?/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($VERB$a)(?:$ADJ$a)(?:$PRP$a)?(?:$NOUN$a|$P$a)?/g);
$Rel =  "SubjL";
DepHead($Rel,"number,person",@temp);
$listTags =~ s/($NOUN${l}concord:1${b}concord:1${r}|$P${l}concord:1${b}concord:1${r})($VERB${l}concord:1${r})($ADJ${l}concord:1${r})($PRP$a)?($NOUN$a|$P$a)?/$2/g;
$listTags =~ s/concord:[01]\|//g;

# DobjR: VERB NOUN|P|DT
(@temp) = ($listTags =~ /($VERB$a)($NOUN$a|$P$a|$DT$a)/g);
$Rel =  "DobjR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($NOUN$a|$P$a|$DT$a)/$1/g;

# CprepR: VERB PRP NOUN|P|VERB<mode:N>|DT
(@temp) = ($listTags =~ /($VERB$a)($PRP$a)($NOUN$a|$P$a|$VERB${l}mode:N\|${r}|$DT$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($PRP$a)($NOUN$a|$P$a|$VERB${l}mode:N\|${r}|$DT$a)/$1/g;

# SubjL: NOUN|P|DT VERB
# Agr: number, person
(@temp) = ($listTags =~ /($NOUN$a|$P$a|$DT$a)($VERB$a)/g);
$Rel =  "SubjL";
DepHead($Rel,"number,person",@temp);
$listTags =~ s/($NOUN${l}concord:1${r}|$P${l}concord:1${r}|$DT${l}concord:1${r})($VERB${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;

# SpecL: [VERB] CONJ<type:S> VERB<mode:[^PNG]>
# NEXT
# DobjR: VERB [CONJ<type:S>] VERB<mode:[^PNG]>
(@temp) = ($listTags =~ /(?:$VERB$a)($CONJ${l}type:S\|${r})($VERB${l}mode:[^PNG]\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
(@temp) = ($listTags =~ /($VERB$a)(?:$CONJ${l}type:S\|${r})($VERB${l}mode:[^PNG]\|${r})/g);
$Rel =  "DobjR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($CONJ${l}type:S\|${r})($VERB${l}mode:[^PNG]\|${r})/$1/g;

############################## END GRAMMAR  ###############################

   if ($ListInit eq $listTags) {
       $STOP = 1;
     }
   }
    #print STDERR "Iterations: $iteration\n";
    ##Fim da iteraÃ§ao (fim de leitura das regras gramaticais)

#########SAIDA CORRECTOR TAGGER

   if ($flag eq "-c") {

       for  ($i=0;$i<=$#Token;$i++) {
          print "$Token[$i]\t";
          $OrdTags{"tag"} = $Tag[$i]; 
          foreach $feat (keys %{$ATTR[$i]}) {
              $OrdTags{$feat} = $ATTR[$i]{$feat};
	  }
          foreach $feat (sort keys %OrdTags) {
                 print "$feat:$OrdTags{$feat}|";
                 delete $OrdTags{$feat};
          }
       print "\n";
       ##Colocar a zero os vectores de cada oraÃ§ao
       delete  $Token[$i];
       delete  $Tag[$i];
       delete  $Lemma[$i];
       delete  $Attributes[$i];
       }
       
    }
#########SAIDA STANDARD DO ANALISADOR 

    elsif ($flag eq "-a") {

      ##Escrever a oraÃ§ao que vai ser analisada:
      print "SENT::$seq\n";
      #print STDERR "LIST:: $listTags\n";
      ####imprimir Hash de dependencias ordenado:
      foreach $triplet (sort {$Ordenar{$a} <=>
                              $Ordenar{$b} }
                         keys %Ordenar ) {
         $triplet =~ s/\(//;
         $triplet =~ s/\)//;
         ($re, $he, $de) =  split (";", $triplet) ;
         if ($re !~ /($DepLex)/) {
           ($he ,$ta1, $pos1) = split ("_", $he);
            $he = $Lemma[$pos1];
            ($de ,$ta2, $pos2) = split ("_", $de);
            $de = $Lemma[$pos2];
            $triplet = "$re;$he\_$ta1\_$pos1;$de\_$ta2\_$pos2" ;
         }
         print "($triplet)\n";
       }
       ##final de analise de frase:
       print "---\n";
     }


  ###SAIDA ANALISADOR COM ESTRUTURA ATRIBUTO-VALOR (full analysis)
   elsif ($flag eq "-fa") {

      ##Escrever a oraÃ§ao que vai ser analisada:
      print "SENT::$seq\n";
      #print STDERR "LIST:: $listTags\n";
      ####imprimir Hash de dependencias ordenado:
      $re="";
      foreach $triplet (sort {$Ordenar{$a} <=>
                              $Ordenar{$b} }
                         keys %Ordenar ) {
           # print "$triplet\n";
         $triplet =~ s/\(//;
         $triplet =~ s/\)//;
         ($re, $he, $de) =  split (";", $triplet) ;

         if ($re !~ /($DepLex)/) { ##se rel nao e lexical
           ($he1, $ta1, $pos1) = split ("_", $he);
            $he1 = $Lemma[$pos1];
            ($de2 ,$ta2, $pos2) = split ("_", $de);
            $de2 = $Lemma[$pos2];
            $triplet = "$re;$he1\_$ta1\_$pos1;$de2\_$ta2\_$pos2" ;
         }
         print "($triplet)\n";
         ($he, ,$ta, $pos) = split ("_", $he);
         print "HEAD::$he\_$ta\_$pos<";
         $ATTR[$pos]{"lemma"} = $Lemma[$pos];
         $ATTR[$pos]{"token"} = $Token[$pos];
         foreach $feat (sort keys %{$ATTR[$pos]}) {
           print "$feat:$ATTR[$pos]{$feat}|" ;
         }
         print ">\n";
         ($de, $ta, $pos) = split ("_", $de);
         print "DEP::$de\_$ta\_$pos<";
         $ATTR[$pos]{"lemma"} = $Lemma[$pos];
         $ATTR[$pos]{"token"} = $Token[$pos];
         foreach $feat (sort keys %{$ATTR[$pos]}) {
           print "$feat:$ATTR[$pos]{$feat}|" ;
         }
         print ">\n";

         if ($re =~ /\//) {
          ($depName, $reUnit) = split ('\/', $re);
          ($reLex, ,$ta, $pos) = split ("_", $reUnit);
          print "REL::$reLex\_$ta\_$pos<";
          $ATTR[$pos]{"lemma"} = $Lemma[$pos];
          $ATTR[$pos]{"token"} = $Token[$pos];
          foreach $feat (sort keys %{$ATTR[$pos]}) {
           print "$feat:$ATTR[$pos]{$feat}|" ;
          }
         print ">\n";
        }
      }
       ##final de analise de frase:
       print "---\n";
     }



      
      ##Colocar numa lista vazia os strings com os tags (listTags) e a oraÃ§ao (seq)
      ##Borrar hash ordenado de triplets
      
      $i=0;
      $j=0;
      $listTags="";
      $seq="";
      foreach $t (keys %Ordenar) {
         delete  $Ordenar{$t};
      }
      for  ($i=0;$i<=$#Token;$i++) {
         delete  $Token[$i];
         delete  $Tag[$i];
         delete  $Lemma[$i];
         delete  $Attributes[$i];
       }
   
    

  } ##fim do elsif


 
}

#print "\n";
print STDERR "Fim do parsing...\n";





sub HeadDep {

    my ($y,$z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

    for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);


           if ($z eq "") {
              $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
              $Rel =  $y ;
              $Dep =  "$Lemma[$n2]_$Tag[$n2]_${n2}" ;

              $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;

            }
            else {
             foreach $atr (@z) {
	
		if ( ($ATTR[$n1]{$atr} ne $ATTR[$n2]{$atr}) && 
                      ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n2]{$atr}  !~  /^[NC0]$/) && ##a modificar: so no caso de que atr = number or genre (N = invariable or neutral)
		     ($ATTR[$n1]{$atr} ne "" && $ATTR[$n2]{$atr} ne  "") ) {
                      
		       $found++;

                }
             }
	
	     # print STDERR "Found: $found\n";
              if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:1\|/;
               $found=0;


               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  $y ;
               $Dep =  "$Lemma[$n2]_$Tag[$n2]_${n2}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;
            }
	   }

    }


}


sub DepHead {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

   # print STDERR "-$y-, -$z-, -@x-\n";

    for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);

            if ($z eq "") {
              $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
              $Rel =  $y ;
              $Dep =  "$Lemma[$n2]_$Tag[$n2]_${n2}" ;

              $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;

            }
            else {
	     
              foreach $atr (@z) {
	      
                 if ( ($ATTR[$n1]{$atr} ne $ATTR[$n2]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n2]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n2]{$atr} ne  "") ) {
		       $found++ ;
                 }
	        
              }
	    
            #  print STDERR "Found: $found\n";
             if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:0\|/;
                $found=0;

             }
             else  {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:1\|/;
               $found=0;


               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  $y ;
               $Dep =  "$Lemma[$n2]_$Tag[$n2]_${n2}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;
	     }
          }

    }


}


sub DepRelHead {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $n3=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

     for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Rel = $x[$m];
            $m++;
            $Head = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Rel =~ /[\w]+_([0-9]+)/);
           ($n3) = ($Dep =~ /[\w]+_([0-9]+)/);

            if ($z eq "") {
               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

            }
            else {
             foreach $atr (@z) {
	
                if ( ($ATTR[$n1]{$atr} ne $ATTR[$n3]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n3]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n3]{$atr} ne  "") ) {
		       $found++ ;
                }
		
             }
	
             if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:1\|/;
               $found=0;


	       $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

             }
	   }

    }


}



sub HeadRelDep {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $n3=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

    for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Rel = $y . "_" . $x[$m];
            $m++;
            $Dep = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Rel =~ /[\w]+_([0-9]+)/);
           ($n3) = ($Dep =~ /[\w]+_([0-9]+)/);


             if ($z eq "") {
               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

                $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

            }
            else {
             foreach $atr (@z) {
	
                 if ( ($ATTR[$n1]{$atr} ne $ATTR[$n3]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n3]{$atr}  !~  /^[NC0]$/) && 
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n3]{$atr} ne  "") ) {
		       $found++ ;
                }
	

             }
	
             if ($found > 0)  {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:1\|/;
               $found=0;


               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

	     }
	   }


    }

}





sub RelDepHead {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $n3=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

     for ($m=0;$m<=$#x;$m++) {
            $Rel = $x[$m];
            $m++;
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Rel =~ /[\w]+_([0-9]+)/);
           ($n3) = ($Dep =~ /[\w]+_([0-9]+)/);

            if ($z eq "") {
               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

            }
            else {
             foreach $atr (@z) {
	
                if ( ($ATTR[$n1]{$atr} ne $ATTR[$n3]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n3]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n3]{$atr} ne  "") ) {
		       $found++ ;
                }
		
             }
	
             if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:1\|/;
               $found=0;


	       $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

             }
	   }

    }


}


sub RelHeadDep {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $n3=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

     for ($m=0;$m<=$#x;$m++) {
            $Rel = $x[$m];
            $m++;
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Rel =~ /[\w]+_([0-9]+)/);
           ($n3) = ($Dep =~ /[\w]+_([0-9]+)/);

            if ($z eq "") {
               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

            }
            else {
             foreach $atr (@z) {
	
                if ( ($ATTR[$n1]{$atr} ne $ATTR[$n3]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n3]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n3]{$atr} ne  "") ) {
		       $found++ ;
                }
		
             }
	
             if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:1\|/;
               $found=0;


	       $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

             }
	   }

    }


}


sub DepHeadRel {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $n3=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

     for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            $m++;
            $Rel = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Rel =~ /[\w]+_([0-9]+)/);
           ($n3) = ($Dep =~ /[\w]+_([0-9]+)/);

            if ($z eq "") {
               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

            }
            else {
             foreach $atr (@z) {
	
                if ( ($ATTR[$n1]{$atr} ne $ATTR[$n3]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n3]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n3]{$atr} ne  "") ) {
		       $found++ ;
                }
		
             }
	
             if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:1\|/;
               $found=0;


	       $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

             }
	   }

    }


}


sub HeadDepRel {

    my ($y, $z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $n3=0;
    my $found=0;
    my @z;

    (@z) = split (",", $z);

     for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Rel = $x[$m];
            $m++;
            $Rel = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Rel =~ /[\w]+_([0-9]+)/);
           ($n3) = ($Dep =~ /[\w]+_([0-9]+)/);

            if ($z eq "") {
               $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

            }
            else {
             foreach $atr (@z) {
	
                if ( ($ATTR[$n1]{$atr} ne $ATTR[$n3]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n3]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n3]{$atr} ne  "") ) {
		       $found++ ;
                }
		
             }
	
             if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n3]_${n3}_<)/$1concord:1\|/;
               $found=0;


	       $Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Rel =  "$y/$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$Lemma[$n3]_$Tag[$n3]_${n3}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n3 ;

             }
	   }

    }


}




sub HeadDep_lex {

    my ($y,$z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $found=0;
    my @z;


    (@z) = split (",", $z);

    for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);

           if ($z eq "") {
              #$Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
              $Head = "$ATTR[$n1]{\"lemma\"}_$Tag[$n1]_${n1}" ;
              $Rel =  $y ;
              #$Dep = "$Lemma[$n2]_$Tag[$n2]_${n2}" ;
              $Dep =  "$ATTR[$n2]{\"lemma\"}_$Tag[$n2]_${n2}" ;

              $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;
              #print STDERR "head::$Head -- dep:::$Dep\n";

	      if (!defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} . "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .  "\@$Token[$n2]";

                  $IDF{$n1}++;

	      }

              elsif (!defined $ATTR_lemma{$n1} && defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} .  "\@$ATTR_lemma{$n2}" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .   "\@$ATTR_token{$n2}";

                  $IDF{$n1}++;

	      }
              elsif (defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} .=   "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} .=   "\@$Token[$n2]";


	      }
              else {

                     $ATTR_lemma{$n1} .=    "\@$ATTR_lemma{$n2}" ;
                     $ATTR_token{$n1} .=    "\@$ATTR_token{$n2}" ;

		
             }

            #print STDERR "$n1::: $ATTR_lemma{$n1} -- $ATTR_token{$n1} \n";

            }
            else {
             foreach $atr (@z) {
	
		if ( ($ATTR[$n1]{$atr} ne $ATTR[$n2]{$atr}) && 
                       ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n2]{$atr}  !~  /^[NC0]$/) &&
		      ($ATTR[$n1]{$atr} ne "" && $ATTR[$n2]{$atr} ne  "") ) {
		       $found++;

                }
             }
	
	     # print STDERR "Found: $found\n";
              if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:1\|/;
               $found=0;

               #$Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Head = "$ATTR[$n1]{\"lemma\"}_$Tag[$n1]_${n1}" ;
               $Rel =  $y ;
               #$Dep = "$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$ATTR[$n2]{\"lemma\"}_$Tag[$n2]_${n2}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;


               if (!defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} . "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .  "\@$Token[$n2]";

                  $IDF{$n1}++;

	       }

               elsif (!defined $ATTR_lemma{$n1} && defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} .  "\@$ATTR_lemma{$n2}" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .   "\@$ATTR_token{$n2}";

                  $IDF{$n1}++;

	       }
               elsif (defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} .=   "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} .=   "\@$Token[$n2]";


	       }
               else {

                     $ATTR_lemma{$n1} .=    "\@$ATTR_lemma{$n2}" ;
                     $ATTR_token{$n1} .=    "\@$ATTR_token{$n2}" ;

		
              }


            }

	   }
           $Lemma[$n1] = $ATTR_lemma{$n1} ;
           $Token[$n1] = $ATTR_token{$n1} ;

    }



}


sub DepHead_lex {

 my ($y,$z, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my $found=0;
    my @z;


    (@z) = split (",", $z);

    for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];

           ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
           ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);

           if ($z eq "") {
              #$Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
              $Head = "$ATTR[$n1]{\"lemma\"}_$Tag[$n1]_${n1}" ;
              $Rel =  $y ;
              #$Dep = "$Lemma[$n2]_$Tag[$n2]_${n2}" ;
              $Dep =  "$ATTR[$n2]{\"lemma\"}_$Tag[$n2]_${n2}" ;

              $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;


               if (!defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} . "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .  "\@$Token[$n2]";

                  $IDF{$n1}++;

	       }

               elsif (!defined $ATTR_lemma{$n1} && defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} .  "\@$ATTR_lemma{$n2}" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .   "\@$ATTR_token{$n2}";

                  $IDF{$n1}++;

	       }
               elsif (defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} .=   "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} .=   "\@$Token[$n2]";


	       }
               else {

                     $ATTR_lemma{$n1} .=    "\@$ATTR_lemma{$n2}" ;
                     $ATTR_token{$n1} .=    "\@$ATTR_token{$n2}" ;

		
              }

	
            }
            else {
             foreach $atr (@z) {
	
		if ( ($ATTR[$n1]{$atr} ne $ATTR[$n2]{$atr}) && 
                        ($ATTR[$n1]{$atr} !~  /^[NC0]$/ && $ATTR[$n2]{$atr}  !~  /^[NC0]$/) &&
                       ($ATTR[$n1]{$atr} ne "" && $ATTR[$n2]{$atr} ne  "")  ) {
		       $found++;

                }
             }
	
	     # print STDERR "Found: $found\n";
              if ($found > 0) {
               	$listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:0\|/;
                $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:0\|/;
                $found=0;
             }
             else {

               $listTags =~ s/($Tag[$n1]_${n1}_<)/$1concord:1\|/;
               $listTags =~ s/($Tag[$n2]_${n2}_<)/$1concord:1\|/;
               $found=0;

               #$Head = "$Lemma[$n1]_$Tag[$n1]_${n1}" ;
               $Head = "$ATTR[$n1]{\"lemma\"}_$Tag[$n1]_${n1}" ;
               $Rel =  $y ;
               #$Dep = "$Lemma[$n2]_$Tag[$n2]_${n2}" ;
               $Dep =  "$ATTR[$n2]{\"lemma\"}_$Tag[$n2]_${n2}" ;

               $Ordenar{"($Rel;$Head;$Dep)"} = $n2 ;

               if (!defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} . "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .  "\@$Token[$n2]";

                  $IDF{$n1}++;

	       }

               elsif (!defined $ATTR_lemma{$n1} && defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} = $ATTR[$n1]{"lemma"} .  "\@$ATTR_lemma{$n2}" ;
                  $ATTR_token{$n1} = $ATTR[$n1]{"token"} .   "\@$ATTR_token{$n2}";

                  $IDF{$n1}++;

	       }
               elsif (defined $ATTR_lemma{$n1} && !defined $ATTR_lemma{$n2} ) {
                  $ATTR_lemma{$n1} .=   "\@$Lemma[$n2]" ;
                  $ATTR_token{$n1} .=   "\@$Token[$n2]";


	       }
               else {

                     $ATTR_lemma{$n1} .=    "\@$ATTR_lemma{$n2}" ;
                     $ATTR_token{$n1} .=    "\@$ATTR_token{$n2}" ;

		
              }


            }
	   }
           $Lemma[$n1] = $ATTR_lemma{$n1} ;
           $Token[$n1] = $ATTR_token{$n1} ;

    }

}



sub Head {

    my ($y, $z, @x) = @_ ;

   return 1

}




sub LEX {
    my $idf=0 ;
    foreach $idf (keys %IDF) {
       #print STDERR "idf = $idf";
       $listTags =~ s/($Tag[$idf]_${idf}${l})lemma:$ATTR[$idf]{"lemma"}/${1}lemma:$ATTR_lemma{$idf}/;
       $listTags =~ s/($Tag[$idf]_${idf}${l})token:$ATTR[$idf]{"token"}/${1}token:$ATTR_token{$idf}/;

       delete $IDF{$idf};
       delete $ATTR_lemma{$idf};
       delete $ATTR_token{$idf};


    }


}


##OperaÃ§oes Corr, Inherit, Add, 

sub Corr {

    my ($z, $y, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my @y="";
    my $atr="";
    my $value="";
    my $info="";
    my $attribute="";
    my $entry="";

    (@y) = split (",", $y);


    if ($z eq "Head") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
            
               ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $entry = "${value}_${n1}_<";

                  if (activarTags($value)) {
                     
                    foreach $attribute (sort keys %TagStr) {
		
		      if (defined $ATTR[$n1]{$attribute}) {
                        $entry .= "$attribute:$ATTR[$n1]{$attribute}|" ;
		      }
                      else {
			$entry .= "$attribute:$TagStr{$attribute}|" ;
		      }
                    }
		  }

	          $entry .= ">";
                  #print STDERR  "--$entry\n" ;
                  $listTags =~ s/$Tag[$n1]_$n1$a/$entry/;
                  $Tag[$n1] = $value;
                  desactivarTags($value);
               }
	       elsif ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
                
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
                
               }
	     }

      }
   }

 

}




sub Inherit {

    my ($z, $y, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my @y="";


    (@y) = split (",", $y);


    if ($z eq "DepHead") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
             foreach $atr (@y) {
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};
	     }

      }
   }

   elsif ($z eq "HeadDep") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
             foreach $atr (@y) {
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }

   elsif ($z eq "DepRelHead") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m +=2;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
             foreach $atr (@y) {
                 
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }

    elsif ($z eq "HeadRelDep") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m +=2;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
             foreach $atr (@y) {
                 
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }

   elsif ($z eq "RelDepHead") {
      for ($m=0;$m<=$#x;$m++) {
	    $m++;
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
             foreach $atr (@y) {
                 
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }

   elsif ($z eq "RelHeadDep") {
      for ($m=0;$m<=$#x;$m++) {
	    $m++;
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
             foreach $atr (@y) {
                 
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }


   elsif ($z eq "DepHeadRel") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
             foreach $atr (@y) {
                 
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }

   elsif ($z eq "HeadDepRel") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
             foreach $atr (@y) {
                 
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }

   elsif ($z eq "DepHead_lex") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
             foreach $atr (@y) {
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};
	     }

      }
   }

   elsif ($z eq "HeadDep_lex") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
             foreach $atr (@y) {
                  $listTags =~ s/($Tag[$n1]_${n1}${l})$atr:$ATTR[$n1]{$atr}/${1}$atr:$ATTR[$n2]{$atr}/;
                  $ATTR[$n1]{$atr} = $ATTR[$n2]{$atr};

	     }

      }
   }
}

sub Add {

    my ($z, $y, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my @y="";
    my $atr="";
    my $value="";
    my $info="";

    (@y) = split (",", $y);


    if ($z eq "DepHead") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
                 # print STDERR "$atr - $value : #$l# - #$r#";
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
                  #print STDERR "$atr - $value ::: #$l# - #$r#";
               }
	     }

      }
   }

   elsif ($z eq "HeadDep") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
	
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
                if ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value; 
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }

	     

      }
   }

   elsif ($z eq "DepRelHead") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m +=2;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }

    
    elsif ($z eq "HeadRelDep") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m +=2;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
	
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
               }
	     }

      }
   }


   elsif ($z eq "RelDepHead") {
      for ($m=0;$m<=$#x;$m++) {
            $m++;
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }


   elsif ($z eq "RelHeadDep") {
      for ($m=0;$m<=$#x;$m++) {
            $m++;
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }


   elsif ($z eq "DepHeadRel") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }


   elsif ($z eq "HeadDepRel") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }



   elsif ($z eq "DepHead_lex") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                   if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
               }
	     }

      }
   } 

   if ($z eq "HeadDep_lex") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       if ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}${info}\|/;
	       }
               else {

                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                 #print STDERR "$atr - $value ::: #$l# - #$r#";
               }
	     }
        
      }
   } 


}


sub Add {

    my ($z, $y, @x) = @_ ;
    my $n1=0;
    my $n2=0;
    my @y="";
    my $atr="";
    my $value="";
    my $info="";

    (@y) = split (",", $y);


 
  
  if ($z eq "Head") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            

            foreach $info (@y) {
               ($atr, $value) = split (":", $info) ;
                if ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
                  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
                  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
               }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
                 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
                 }
               }
             }



      }
   }

   elsif ($z eq "DepHead") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;

               ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                   $Tag[$n1] = $value;
               }
	       elsif ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
                 # print STDERR "$atr - $value : #$l# - #$r#";
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
                  #print STDERR "$atr - $value ::: #$l# - #$r#";
               }
	     }

      }
   }

   elsif ($z eq "HeadDep") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
	
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
               
                ##change the PoS tag:
                if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value; 
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }

	     

      }
   }

   elsif ($z eq "DepRelHead") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m+=2;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                ##change the PoS tag:
                if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                   $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }

    
    elsif ($z eq "HeadRelDep") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m +=2;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
	
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
               }
	     }

      }
   }


   elsif ($z eq "RelDepHead") {
      for ($m=0;$m<=$#x;$m++) {
            $m++;
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }


   elsif ($z eq "RelHeadDep") {
      for ($m=0;$m<=$#x;$m++) {
            $m++;
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                ##change the PoS tag:
                if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }


   elsif ($z eq "DepHeadRel") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
	       elsif ($listTags  =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }


   elsif ($z eq "HeadDepRel") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                 ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /($Tag[$n1]_${n1}${l})$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                 if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		 }
                 if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		 }
               }
	     }
      }
   }



   elsif ($z eq "DepHead_lex") {
      for ($m=0;$m<=$#x;$m++) {
            $Dep = $x[$m];
            $m++;
            $Head = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
              
                ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
	       elsif ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}$info\|/;
	       }
               else {
                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                   if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
               }
	     }

      }
   } 

   if ($z eq "HeadDep_lex") {
      for ($m=0;$m<=$#x;$m++) {
            $Head = $x[$m];
            $m++;
            $Dep = $x[$m];
            ($n1) = ($Head =~ /[\w]+_([0-9]+)/) ;
            ($n2) = ($Dep =~ /[\w]+_([0-9]+)/);  
		
            foreach $info (@y) {
	       ($atr, $value) = split (":", $info) ;
	       
                ##change the PoS tag:
               if ($atr =~ /^tag/) {
                  $ATTR[$n1]{$atr} = $value;
                  $listTags =~ s/$Tag[$n1]/$value/;
                  $Tag[$n1] = $value;
               }
               elsif ($listTags =~ /$Tag[$n1]_${n1}${l}$atr/) {
                  $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                  $listTags =~ s/($Tag[$n1]_${n1}${l})${atr}:[^|]*\|/${1}${info}\|/;
	       }
               else {

                 $listTags =~ s/($Tag[$n1]_${n1}_<)/${1}$info\|/;
                 $ATTR[$n1]{$atr} = $value;
                  if ($atr eq "lemma") {
                    $Lemma[$n1] = $ATTR[$n1]{"lemma"};
		  }
                  if ($atr eq "token") {
                    $Token[$n1] = $ATTR[$n1]{"token"};
		  }
                 #print STDERR "$atr - $value ::: #$l# - #$r#";
               }
	     }
        
      }
   } 


}



sub negL {

    my ($x) = @_ ;
    my $expr="";
    my $ref="";
    my $CAT="";
  
    ($CAT, $ref) = split ("_", $x);  
    $expr = "(?<!${CAT})\\_$ref";
  

   return $expr
}

sub negR {

    my ($x) = @_ ;
    my $expr="";
    my $ref="";
    my $CAT="";
 
    ($CAT, $ref) = split ("_", $x); 
    $expr = "?!${CAT}\\_$ref";
 
   return $expr
}



sub activarTags {

   my ($x) = @_ ; 

   ##shared attributes:
   # $TagStr{"tag"} = 0;
    $TagStr{"lemma"} = 0;
    $TagStr{"token"} = 0;
  
   if ($x =~ /^PRO/) {
  
      $TagStr{"type"} = 0;
      $TagStr{"person"} = 0;
      $TagStr{"genre"} = 0;
      $TagStr{"number"} = 0;
      $TagStr{"case"} = 0;
      $TagStr{"possessor"} = 0;
      $TagStr{"politeness"} = 0;
      $TagStr{"pos"} = $n1;
      return 1 ;
   }

     ##conjunctions:
    elsif ($x =~ /^C/) {
      
       $TagStr{"type"} =  0;
       $TagStr{"pos"} = $n1;
        return 1 ;
    }

    ##interjections:
    elsif ($x =~ /^I/) {
   
       $TagStr{"type"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
    }

   ##numbers
   elsif ($x =~  /^CARD/) {
       
       $TagStr{"number"} = "P";
       $TagStr{"person"} = 0;
       $TagStr{"genre"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
   }


    elsif ($x =~ /^ADJ/) {
       
       $TagStr{"type"} = 0;
       $TagStr{"degree"} = 0;
       $TagStr{"genre"} = 0;
       $TagStr{"number"} = 0;
       $TagStr{"function"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
   }


   elsif ($x =~ /^ADV/) {
       
       $TagStr{"type"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
   }


   elsif ($x =~ /^PRP/) {
       
       $TagStr{"type"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
   }


   elsif ($x =~ /^NOUN/) {
      
       $TagStr{"type"} = 0 ;
       $TagStr{"genre"} = 0;
       $TagStr{"number"} = 0;
       $TagStr{"person"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
   }

   elsif ($x =~ /^DT/) {
 
       $TagStr{"type"} = 0;
       $TagStr{"person"} = 0;
       $TagStr{"genre"} = 0;
       $TagStr{"number"} = 0;
       $TagStr{"possessor"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;
   }

  ##mudar tags nos verbos:
   elsif ($x =~ /^VERB/) {


       $TagStr{"type"} = 0;
       $TagStr{"mode"} = 0;
       $TagStr{"tense"} = 0;
       $TagStr{"person"} = 0;
       $TagStr{"number"} = 0;
       $TagStr{"genre"} = 0;
       $TagStr{"pos"} = $n1;
       return 1 ;  
   }
   else {
     return 0
   }

}


sub desactivarTags {

   my ($x) = @_ ; 

      delete $TagStr{"type"} ;
      delete $TagStr{"person"};
      delete $TagStr{"genre"} ;
      delete $TagStr{"number"} ;
      delete $TagStr{"case"} ;
      delete $TagStr{"possessor"} ;
      delete $TagStr{"politeness"} ;
      delete $TagStr{"mode"} ;
      delete $TagStr{"tense"} ;
      delete $TagStr{"function"} ;
      delete $TagStr{"degree"} ;
      delete $TagStr{"pos"} ;     
 
      delete $TagStr{"lemma"} ;
      delete $TagStr{"token"} ;
     # delete $TagStr{"tag"} ;
     return 1
}
