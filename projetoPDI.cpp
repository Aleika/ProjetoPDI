#include <iostream>
#include <opencv2/opencv.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace std;
using namespace cv;

int i = 0, g=0;
vector<Mat> images;
vector<Mat> imgBorrada;
vector<Mat> vetorFinal;

void detectSmile(Mat& img);
void limparLista();
void adicionarVetorFinal(Mat& img);
void efeito(Mat& img);
void mostrarSalvarResult();

CascadeClassifier cascadeFace, cascadeMouth;

String cascadeFaceName  = "haarcascades/haarcascade_frontalface_alt.xml";
String cascadeMouthName = "haarcascades/haarcascade_smile.xml";
//String cascadeMouthName = "haarcascades/haarcascade_mcs_mouth.xml";

int main( int argc, const char** argv ){
 
    String folder = "outro_teste/*.jpg";
    vector<String> filenames;
    Mat image, img;
  
    if( !cascadeFace.load( cascadeFaceName )) {
	cerr << "ERRO: Nao carregou filtro em cascata facefrontal" << endl;
	return -1;
    }
    
   if( !cascadeMouth.load( cascadeMouthName )) {
	cerr << "ERRO: Nao carregou filtro em cascata mouth" << endl;
	return -1;
    }

    //Função responsável por salva as imagens no diretório especificado
    // em folder
    glob(folder, filenames);

    //são percorridas todas as imagens da pasta
    for (size_t i = 0; i < filenames.size(); ++i)
    {
	//'image' recebe a imagem lida do diretório 
        image = imread(filenames[i]);
      
	detectSmile(image);
    }

    mostrarSalvarResult();
 
  return 0;
}

//Função responsável por detectar o sorriso 
void detectSmile( Mat& img){
  vector<Rect> faces;
  
  Mat gray;
  cascadeFace.detectMultiScale(img, // imagem para deteccao
				   faces, // vetor com os retangulos encontrados
				   1.1, // escala de multiresolucao
				   3, // numero de vizinhos que cada candidato a retangulo
				   // devera contemplar. evita multiplas deteccoes parecidas
				   // na mesma regiao
				   0 | CV_HAAR_FIND_BIGGEST_OBJECT, // parametros (normalmente nao usados)
				   Size(30, 30) ); // mínimo tamanho para deteccao de um objeto
  
  for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++ ){
	Mat imgROI;
	vector<Rect> nestedObjects;
	
	
	if( cascadeMouth.empty() )
	  continue;

	// posicao aproximada da boca em relacao a face...
	Rect mouthROI = Rect(r->x, r->y + (r->height/1.5), r->width, r->height/2.5);    


	imgROI = img(mouthROI);
	
	cascadeMouth.detectMultiScale(
					imgROI,
					nestedObjects,
					1.1,
					10,
					0 | CV_HAAR_FIND_BIGGEST_OBJECT,
					Size(30, 30) );


	for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ ){
		//Desenha um retângulo marcando a região da boca		
		rectangle(img,  
				Point(r->x + nr->x, r->y + (r->height/1.5) + nr->y  ),  
				Point(r->x + nr->x + nr->width, r->y + (r->height/1.5) + nr->y + nr->height),  
				CV_RGB(255, 0, 255), 1, 8, 0);
		
		//Cada imagem que é detectado sorriso é adicionada ao vetor de imagens 'images'		
		images.push_back(img);
		
		//Trecho de código responsável por salvar as imagens que possuem sorriso na pasta 'aqui'
		//As imagens são salvas como 'image1.jpg' - 'image2.jpg' ... e assim por diante
		string s;
		s="aqui/image"+to_string(i)+".jpg";
		imwrite(s.c_str(), img);
		i++;
		
		// É aplicado o efeito de borramento a cada imagem
		efeito(img);
		
		//O vetor que armazena o borramento de cada imagem é esvaziado para que ele receba
		// as imagens de borramento de um nova imagem
		limparLista();
	}
  }

}


//O efeito de borramento consiste em aplicar 6 diferentes intensidades de borramento
//em cada imagem recebida pela função
void efeito(Mat& img){

	Mat imgBor;// matriz que irá receber a imagem borrada
	int j,k;

	for(j=0;j<6; j++){

		//Função que aplica borramento na imagem 'img' e salva resultado em 'imgBor'
		// O último parâmetro especifica a intensidade do borramento.
		// Quanto maior o valor, maior o borramento.
		GaussianBlur(img, imgBor, Size(0, 0), j+1);
		
		//Cada imagem resultante do borramento é adicionada ao vetor 'imgBorrada'		
		imgBorrada.push_back(imgBor);

		//Trecho de código responsável por salvar as imagens que possuem sorriso na pasta 'borradas'
		//As imagens são salvas como 'image1.jpg' - 'image2.jpg' ... e assim por diante
		string s;
		s="borradas/image"+to_string(g)+".jpg";
		imwrite(s.c_str(), imgBorrada[j]);
		g++;
	}

	//Trecho de código para teste	
	/*for(k =0; k< size; k++){
		string s;
		s="teste/image"+to_string(k)+".jpg";
		imwrite(s.c_str(), imgBorrada[k]);
		waitKey(0);
	}*/

	adicionarVetorFinal(img);
}

//Função que limpa o vetor 'imgInicialBorrada'
void limparLista(){

	int j;
        int size = imgBorrada.size() ;
	for(j=0 ; j< size; j++){

		imgBorrada.pop_back();
	}
}

//O 'vetorFinal' armazenará todas as imagens que irão compor o efeito de exibição no video
// O efeito que se deseja gerar é: Serão mostradas imagens com borramento (primeira imagem com pouco borramento 
//e as próximas com efeito mais forte de borramento) -> imagens sem borramento -> novamente imagem com borramento 
//(primeira imagem com muito borramento e as próximas com efeito mais leve de borramento).
void adicionarVetorFinal(Mat& img){
	int j ;
        int size = imgBorrada.size();

	//teste	
	/*for(int j =0; j < size ;j++){
		string s;
		s="image"+to_string(j);
		imshow(s.c_str(), imgInicialBorrada[j]);
		waitKey(0);
	}*/

	// São adicionadas ao vetor 'vetorFinal' as imagens com ordem crescente
	// de intensidade de borramento
	for(j=size-1; j>=0;j--){
		vetorFinal.push_back(imgBorrada[j]);
	}

	//Em sequência, são adicionadas imagens sem borramento ao vetor 'vetorFinal'
	// São adicionadas várias imagens para dar um efeito de 'maior tempo de visualização'
	// da imagem normal (sem borramento)
	for(j=0; j<6;j++){
		vetorFinal.push_back(img);
	}

	// Por fim, São adicionadas ao vetor 'vetorFinal' as imagens com ordem decrescente
	// de intensidade de borramento
	for(j=0;j<size;j++){
		vetorFinal.push_back(imgBorrada[j]);
	}

}

//Salva as imagens contidas no 'vetorFinal' em um diretório
void mostrarSalvarResult(){

	int size = vetorFinal.size();

	for(int j =0; j < size ;j++){

		//Trecho de código responsável por salvar as imagens que possuem sorriso na pasta 'aqui'
		//As imagens são salvas como 'image1.jpg' - 'image2.jpg' ... e assim por diante
		string s;
		s="final/image"+to_string(j)+".jpg";
		imwrite(s.c_str(), vetorFinal[j]);
		//imshow("imagem", vetorFinal[j]);
		//waitKey(0);
	}

}

