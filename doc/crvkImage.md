
# manipulos / handles 
crvkImage / crvkImageStatic / crvkImageStaging

# crvkImage
Estrutura de base para imagens vulkan, fornece a estrutura mais basica da imagem, apenas criando o handler, allocando memoria para a imagem,
e criando o view, forma mais flexivel, onde você tem total controle externamente do gerenciamento de onde e como a imagem tem seu conteudo copiado e utilizado.  

# crvkImageStatic
Estrutura de imagem com buffer interno de operações, utilidade que auxilia na gestão da imagem, com estrutura interna para gestão de copia de conteudo,
e semaphoros para controle das operações.

# crvkImageStaging
Estrutura mais dinamica, contendo um buffer interno para copia de conteudo, semaphoros para copia e um buffer de comandos interno para gestão,
busca operar de forma mais semelhande possivel ao openGL permitindo copia diretametne para dentro da textura ( atravez de seu buffer de estagio interno ), 
sem a necessidade de muitas operações extenas como buffers de comando e sincronização. 
