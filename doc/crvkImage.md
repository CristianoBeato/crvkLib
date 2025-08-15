
# manipulos / handles 
crvkImage / crvkImageStatic / crvkImageStaging

# crvkImage
Estrutura de base para imagens vulkan, fornece a estrutura mais basica da imagem, apenas criando o handler, allocando memoria para a imagem,
e criando o view, forma mais flexivel, onde você tem total controle externamente do gerenciamento de onde e como a imagem tem seu conteudo copiado e utilizado.  

/

The base structure for Vulkan images provides the most basic image structure, simply creating the handler, allocating memory for the image, and creating the view. This is a more flexible way to manage where and how the image's content is copied and used.

# crvkImageStatic
Estrutura de imagem com buffer interno de operações, utilidade que auxilia na gestão da imagem, com estrutura interna para gestão de copia de conteudo,
e semaphoros para controle das operações.

/

Image structure with internal operations buffer, a utility that assists in image management, with an internal structure for managing content copying, and semaphores for controlling operations.

# crvkImageStaging
Estrutura mais dinamica, contendo um buffer interno para copia de conteudo, semaphoros para copia e um buffer de comandos interno para gestão,
busca operar de forma mais semelhande possivel ao openGL permitindo copia diretametne para dentro da textura ( atravez de seu buffer de estagio interno ), 
sem a necessidade de muitas operações extenas como buffers de comando e sincronização. 

/

A more dynamic structure, containing an internal buffer for copying content, semaphores for copying, and an internal command buffer for management,
it seeks to operate as similarly as possible to OpenGL, allowing direct copying into the texture (through its internal staging buffer),
without the need for many external operations such as command buffers and synchronization.