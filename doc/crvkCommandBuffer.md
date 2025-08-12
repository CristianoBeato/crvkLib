# manipulos / handles
crvkCommandBuffer / crvkCommandBufferRoundRobin

# crvkCommandBuffer
Estrutura basica de gravação de commandos, uma abstração simples do funcionamento do buffer de comandos do vulkan.
/
Basic command recording structure, a simple abstraction of how the Vulkan command buffer works.

# crvkCommandBufferRoundRobin

Essa estrutura gerencia um conjunto de command buffers Vulkan organizados em um esquema round-robin para registrar comandos de renderização de forma contínua e não bloquear a execução.
  
Ela mantém:
Uma cadeia de command buffers (m_commandBuffers) que são utilizados de forma cíclica.
Um semaphore do tipo timeline (m_doneSemaphore) para sincronizar a conclusão do uso de cada buffer.
Ponteiros para o dispositivo (m_device) e fila de execução (m_queue) associados.
Variáveis de controle como o índice do buffer atual (m_currentBuffer) e o valor atual da timeline (m_timelineValue).
Funcionamento:
Em cada frame, o crvkCommandBufferRoundRobin seleciona o próximo command buffer da cadeia usando o esquema round-robin (incrementando m_currentBuffer e reiniciando no final da lista).
Antes de reusar um buffer, ele aguarda via timeline semaphore que a GPU tenha finalizado seu uso anterior, garantindo que não haja sobrescrita de comandos ainda pendentes.
O command buffer atual é resetado e preparado para registrar os comandos do novo frame.
Assim que a gravação termina, o buffer é enviado para execução na GPU via m_queue.
O timeline semaphore é sinalizado com um novo valor (m_timelineValue), marcando o ponto de sincronização para quando a GPU concluir esse trabalho.
Enquanto o buffer enviado está aguardando execução, o sistema já avança para preparar o próximo buffer, evitando ociosidade e mantendo alta taxa de renderização.
Esse design permite que o registro de comandos do próximo frame comece imediatamente após enviar o anterior, mantendo a CPU e a GPU trabalhando em paralelo com mínima latência.
/
This structure manages a set of Vulkan command buffers organized in a round-robin fashion to continuously record rendering commands without blocking execution.
It maintains:
A chain of command buffers (m_commandBuffers) that are used cyclically.
A timeline semaphore (m_doneSemaphore) to synchronize the completion of each buffer.
Pointers to the associated device (m_device) and execution queue (m_queue).
Control variables such as the current buffer index (m_currentBuffer) and the current timeline value (m_timelineValue).
How it works:
At each frame, crvkCommandBufferRoundRobin selects the next command buffer in the chain using a round-robin fashion (incrementing m_currentBuffer and restarting at the end of the list).
Before reusing a buffer, it waits via a timeline semaphore for the GPU to finish its previous use, ensuring that no pending commands are overwritten.
The current command buffer is reset and prepared to record the commands for the new frame.
As soon as recording is complete, the buffer is sent to the GPU for execution via m_queue.
The timeline semaphore is signaled with a new value (m_timelineValue), marking the synchronization point for when the GPU completes this work.
While the sent buffer is awaiting execution, the system moves on to preparing the next buffer, avoiding idle time and maintaining a high rendering rate.
This design allows command recording for the next frame to begin immediately after sending the previous one, keeping the CPU and GPU working in parallel with minimal latency.
```mermaid
classDiagram
    class crvkCommandBufferRoundRobin 
    {
        - uint32_t m_numBuffers;
        - uint32_t m_currentBuffer;
        - uint64_t m_timelineValue;
        - VkCommandBuffer* m_commandBuffers;
        - VkSemaphore m_doneSemaphore;
    }

class VkRenderPass
cass VkFramebuffer
class VkPipeline
class crvkDeviceQueue

VkPipeline --> crvkCommandBufferRoundRobin : BindPipeline
VkRenderPass --> crvkCommandBufferRoundRobin : BeginRenderPass
VkFramebuffer --> crvkCommandBufferRoundRobin : BeginRenderPass
crvkCommandBufferRoundRobin --> crvkDeviceQueue : vkQueueSubmit
   ```
