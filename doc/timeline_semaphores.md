## VK_KHR_timeline_semaphore

# mainpulo:
# handle:
    VkSemaphore
    
# inicialização:
# initialization:
"
    VkSemaphoreTypeCreateInfo timelineCreateInfo{};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = 0;
    semaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( device, &semaphoreCI, allocationCallbacks, &semaphore );
"

# descrição:
    Um semaphore de timeline é um tipo de VkSemaphore que tem um valor numérico associado a ele (chamado "valor de timeline").
    Ele permite sincronização mais flexível entre comandos enviados para a GPU, sem necessidade de múltiplos semáforos tradicionais para cada ponto.

    É diferente do VkSemaphore binário tradicional (que só tem "sinalizado" ou "não sinalizado").

# description:
    A timeline semaphore is a type of VkSemaphore that has a numeric value associated with it (called a "timeline value").
    It allows for more flexible synchronization between commands sent to the GPU, without the need for multiple traditional semaphores for each point.

    It differs from the traditional binary VkSemaphore (which only has "signaled" or "unsignaled").

# Operação:
    espera por semaforos da lista de espera atingir o valor 'N' => executa comandos => sinaliza semaforos da list de sinal com o valor 'N'

# Operation:
    wait for semaphores from the wait list to reach the value 'N' => execute commands => signal semaphores from the signal list with the value 'N'