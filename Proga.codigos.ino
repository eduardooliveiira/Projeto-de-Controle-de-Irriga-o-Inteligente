// Projeto de Controle de Irrigação Inteligente
// Autor: Eduardo Fernando de Oliveira
// Projeto para a disciplina de Objetos Inteligentes e Conectados da Universidade Persbiteriana Mackenzie

// Incluindo as Bibliotecas da Placa NodeMCU ESP8266 para estabelecer conexão via WiFi
# inclui  < ESP8266WiFi.h >
# include  < PubSubClient.h >

// Definindo as variáveis ​​e os pinos e entradas correspondentes de cada componente
# define  DEBUG
# define  pino_sinal_analogico A0

# define  pino_led_vermelho  13  // 5
# define  pino_led_verde  12  // 7
# define  pino_bomba  14  // 8 14

// Variáveis
int valor_analógico;
String strMSG = " 0 " ;
char mensagem[ 30 ];

// Informações da Rede WIFI para conexão com o CloudMQTT, que estabelece conectividade com o Broker que fará o monitoramento dos Solos
const  char * ssid = " casazul " ;             // SSID da rede WIFI
const  char * senha =   " internetdanieldantas " ;    // senha da rede wifi
// Informações da Instância do broker MQTT
const  char * mqttServer = " postman.cloudmqtt.com " ;   // servidor
const  char * mqttUser = " aeleozfk " ;              // usuário
const  char * mqttPassword = " J0MSDy4RiaK8 " ;      // senha
const  int mqttPort = 16157 ;                     // porta
const  char * mqttTopicSub = " jardim/bomba " ;      // tópico que será assinado no Broker

WiFiClient espClient;
cliente PubSubClient (espClient);

// Definindo as entradas correspondentes aos pinos e os Prints de Resposta para o monitor serial
void  configuração () {
  Serial. começar ( 9600 );
  pinMode (pino_sinal_analogico, INPUT);
  pinMode (pino_led_vermelho, SAÍDA);
  pinMode (pino_led_verde, SAÍDA);
  pinMode (pino_bomba, SAÍDA);

  Wi-fi. começar (ssid, senha);

  Serial. print ( " Entrando no Setup " );

  while (WiFi. status () != WL_CONNECTED) {
    atraso ( 500 );
# ifdef DEBUG
    Serial. println ( " Aguarde! Conectando ao WiFi... " );
# endif
  }
# ifdef DEBUG
  Serial. println ( " Conectado na rede WiFi com sucesso! " );
# endif

  cliente. setServer (mqttServer, mqttPort);
  cliente. setCallback (retorno de chamada);

  while (!client. conectado ()) {
# ifdef DEBUG
    Serial. println ( " Conectando ao Broker MQTT... " );
# endif

    if (client. connect ( " ESP8266Client " , mqttUser, mqttPassword )) {
# ifdef DEBUG
      Serial. println ( " Conectado com sucesso " );
# endif

    } senão {
# ifdef DEBUG
      Serial. print ( " falha estado   " );
      Serial. imprimir (cliente. estado ());
# endif
      atraso ( 2000 );
    }
  }

  // subscreve nenhum tópico
  cliente. subscrever (mqttTopicSub);

}

void  loop () {

  if (!cliente. conectado ()) {
    Serial. print ( " Passou no Conectado " );
    reconectar ();

  }
 
  Serial. imprimir (valor_analógico);
  // Lendo o valor do pino A0 do sensor para detecção
  valor_analogico = analogRead (pino_sinal_analogico);

  // verificando o valor da porta analógica no serial monitor
  Serial. print ( " Porta analogica: " );
  Serial. imprimir (valor_analógico);

  // Definindo o parâmetro para um solo devidamente úmido, acendendo o LED VERDE
    if (valor_analogico >= 0 && valor_analogico < 500 )
    {
      Serial. println ( " Status: Solo umido " );
    
      digitalWrite (pino_led_vermelho, LOW);
      digitalWrite (pino_led_verde, ALTO);
      digitalWrite (pino_bomba, ALTO);
  
      // Enviando Mensagem ao Corretor
      sprintf (mensagem, " 0 " ); // Definindo o valor zero como parâmetro para o Broker exibir uma imagem "vermelha"
      Serial. print ( " Mensagem enviada: " );
      Serial. println (mensagem);
      cliente. publique ( " jardim/bomba " , mensagem);
      Serial. println ( " Mensagem enviada com sucesso... " );
  
    }

 
  // Definindo o parâmetro para um Solo Seco, acendendo o LED VERMELHO
    if (valor_analogico >= 500 && valor_analogico <= 1024 )
    {
      Serial. println ( " Status: Solo seco " );
      
      digitalWrite (pino_led_verde, BAIXO);
      digitalWrite (pino_led_vermelho, ALTO);
      digitalWrite (pino_bomba, BAIXO);
  
       // Enviando Mensagem ao Corretor
      sprintf (mensagem, " 1 " ); // Definindo o valor UM como parâmetro para o Broker exibir uma imagem "verde"
      Serial. print ( " Mensagem enviada: " );
      Serial. println (mensagem);
      cliente. publique ( " jardim/bomba " , mensagem);
      Serial. println ( " Mensagem enviada com sucesso... " );
  
    }
  atraso ( 10000 );

  cliente. laço ();

}

void  callback ( char * topic, byte * payload, unsigned  int length) {

  // Armazenando mensagens recebidas em uma string
  carga útil[comprimento] = ' \0 ' ;
  strMSG = String (( char *)payload);

# ifdef DEBUG
  Serial. print ( " Mensagem chegou ao tópico: " );
  Serial. println (tópico);
  Serial. print ( " Mensagem: " );
  Serial. imprimir (strMSG);
  Serial. println ();
  Serial. println ( " ----------------------- " );
# endif

}

// Função para reconectar ao servidor MQTT
void  reconectar () {
  // Enquanto estiver desconectado
  while (!client. conectado ()) {
# ifdef DEBUG
    Serial. print ( " Tentando conectar ao servidor MQTT " );
# endif

    bool conectado = strlen (mqttUser) > 0 ?
                     cliente. connect ( " ESP8266Client " , mqttUser, mqttPassword):
                     cliente. connect ( " ESP8266Cliente " );

    if (conectado) {
# ifdef DEBUG
      Serial. println ( " Conectado! " );
# endif
      // Subscrever nenhum tópico
      cliente. subscrever (mqttTopicSub, 1 ); // nivel de qualidade: QoS 1
    } senão {
# ifdef DEBUG
      Serial. println ( " Falha ao conectar.Code: " );
      Serial. println ( String (client. estado ()). c_str ());
      Serial. println ( " Tentando novamente em 10 s " );
# endif
      // Aguarda 10 segundos
      atraso ( 5000 );
    }
  }
}
