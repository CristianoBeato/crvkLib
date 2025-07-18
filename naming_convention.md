# Convenção de Nomes — Projeto C++ de Beato

## Variáveis Membro (`m_`)
- Prefixo: `m_`
- Descrição: Membros da instância da classe.
- Exemplo:
  ```cpp
  int m_id;
  std::string m_nome;
  ```

## Váriaveis Globais ( `s_` ou `k_` )
- Prefixo: `k_` para contantes ou valores de dempo de compilação
- Prefixo: `s_` para variaveis estaticas
- Exemplo:
  ```cpp
  static int        s_count;
  static const      k_size = 128;
  static constexpr  k_total = 1024;
  ```
/
## Classes 
- Estilo: PascalCase "NomeDaClasse"
- Prefixo: do dev + nome da classse ( crNomeDaClasse ) ou ( btNomeDaClasse )

/

# Naming Convention — Beato's C++ Project

## Member Variables (`m_`)
- Prefix: `m_`
- Description: Members of the class instance.
- Example:
```cpp
int m_id;
std::string m_name;
```

## Global Variables (`s_` or `k_`)
- Prefix: `k_` for constants or compile-time values
- Prefix: `s_` for static variables
- Example:
```cpp
static int s_count;
static const k_size = 128;
static constexpr k_total = 1024; ```

## Classes
- Style: PascalCase "ClassName"
- Prefix: do dev + class name ( crClassName ) or ( btClassName )