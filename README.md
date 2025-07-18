# crvkLib
Uma pequena biblioteca modular Vulkan + SDL. / A small modular Vulkan + SDL library.

Desenvolvida com o objetivo de **trazer a simplicidade e fluidez do OpenGL** para o universo detalhista (e às vezes cruel) do Vulkan — tudo isso mantendo o controle de baixo nível que o Vulkan oferece.

Feita pra quem quer desenvolver rápido, entender o pipeline, e não perder tempo com toneladas de boilerplate.

## 🎯 Objetivos / Objectives:

- ✅ Reduzir a verbosidade típica do Vulkan
- ✅ Fornecer estruturas reutilizáveis com prefixo `crvk_`
- ✅ Manter o código limpo, direto e fácil de estender
- ✅ Rodar com SDL3 para máxima portabilidade (Windows/Linux)
- ✅ Foco em aprendizado, prototipagem e dev independente

/

- ✅ Reduce typical Vulkan verbosity
- ✅ Provide reusable frameworks with the `crvk_` prefix
- ✅ Keep code clean, straightforward, and easy to extend
- ✅ Run with SDL3 for maximum portability (Windows/Linux)
- ✅ Focus on learning, prototyping, and independent development

---

## 🚀 Funcionalidades / Features:

- Inicialização rápida de Vulkan (instância, dispositivo, filas).
- Suporte a SDL para criação de janelas e contextos Vulkan.
- Gestão de Swapchain simplificada.
- Gerenciamento de buffers e imagens com wrappers leves.
- Estruturas prontas para comandos, sincronização e render pass.
- Organização e estruturação simples e acessivel.
- Depuração simples.

/

- Fast Vulkan initialization (instance, device, queues).
- SDL support for creating Vulkan windows and contexts.
- Simplified swapchain management.
- Buffer and image management with lightweight wrappers.
- Ready-made structures for commands, synchronization, and render pass.
- Simple and accessible organization and structuring.
- Simple debugging.

---

## 📦 Dependências / Dependencies:

- [Vulkan SDK](https://vulkan.lunarg.com/)
- [SDL3](https://github.com/libsdl-org/SDL)

---

## 🧠 Filosofia / Philosophy:

> **"Menos é mais" — especialmente quando o mais são 500 linhas de `VkCreateXxxInfo`.**

Essa biblioteca é feita pra quem quer:
- Migrar do OpenGL ( Esquecido pela Khronos ).
- Aprender Vulkan de forma mais limpa.
- Montar um projeto gráfico moderno.
- Reaproveitar código entre protótipos.

/

> **"Less is more" — especially when that more is 500 lines of `VkCreateXxxInfo`.**

This library is made for those who want to:
- Migrate from OpenGL (forgotten by Khronos ).
- Learn Vulkan more cleanly.
- Build a modern graphics project.
- Reuse code between prototypes.

---

## 🔧 Em andamento / Work in progress:

Este projeto ainda está em desenvolvimento.  
Sugestões, ideias ou bugs? Fique à vontade pra abrir uma *issue* ou PR.

Feito com 💻 por Beato, movido a café e vontade de desenhar triângulos sem tela preta ou surtar.

/

This project is still under development.

Suggestions, ideas, or bugs? Feel free to open an issue or PR.

Made with 💻 by Beato, fueled by coffee and the desire to draw triangles without a black screen or freaking out.

---

## 📄 Licença / License:

Este projeto está licenciado sob a MIT License com cláusula de atribuição.  
Você pode usá-lo livremente em projetos pessoais e comerciais, desde que dê os devidos créditos.  
Veja o arquivo [LICENSE](./LICENSE) para mais detalhes.

/ 

This project is licensed under the MIT License with attribution.
You can use it freely in personal and commercial projects, as long as you give proper credit.
See the [LICENSE](./LICENSE) file for more details.
