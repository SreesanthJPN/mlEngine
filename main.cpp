#include<iostream>
#include<vector>
#include<2dTiles.h>
#include<bias.h>
#include<threadss.h>
#include<activations.h>
#include<stdexcept>


class newModel{

    private:

    std::vector<int> layers;

    double*** weights = new double**[layers.size()];
    double*** gradients = new double**[layers.size()];
    double*** bias = new double**[layers.size()];
    double*** biasGrads = new double**[layers.size()];
    double*** layerOutputs = new double**[layers.size()];

    int inputColSize;
    int inputRowSize;

    std::vector<activation> activations;

    threadPool* tP = createThreadPool();
    activationThreadPool* aTP = createActivationThreadPool();
    biasPool* bP = createBiasPool();

    public:

    newModel(std::vector<int> lyrs, int inRows, int inCols, std::vector<activation> acts) : layers(lyrs), inputColSize(inCols), inputRowSize(inRows), activations(acts){

        if(lyrs.size() != activations.size()) throw std::runtime_error("Activations Size Mismatch");

        for(size_t i = 0; i < layers.size(); i++){
            int prevDim = (i == 0) ? inputColSize : layers[i-1];

            std::cout<<"Weight matrix Size "<<layers[i]<<"---"<<prevDim<<std::endl;
            weights[i] = createMatrix(layers[i], prevDim);
            gradients[i] = createMatrix(layers[i], prevDim);

            std::cout<<"bias matrix Size "<<layers[i]<<"---"<<1<<std::endl;
            bias[i] = createMatrix(layers[i], 1);
            biasGrads[i] = createMatrix(layers[i], 1);

            layerOutputs[i] = createzMatrix(inputRowSize, layers[i]);

            std::cout<<"\n";
        }
    }

    // Runs `input` (inputRowSize x inputColSize) through every layer:
    // tiled multithreaded matmul -> bias broadcast -> activation.
    // Returns a pointer to the model-owned output buffer of the last layer
    // (inputRowSize x layers.back()) -- valid until the next forward() call
    // or model destruction.
    double** forward(double** input){
        double** current = input;

        for(size_t i = 0; i < layers.size(); i++){
            int prevDim = (i == 0) ? inputColSize : layers[i-1];

            for(int r = 0; r < inputRowSize; r++)
                for(int c = 0; c < layers[i]; c++)
                    layerOutputs[i][r][c] = 0.0;

            tile* inTiles = createTiles(inputRowSize, prevDim);
            tile* wTiles  = createTiles(layers[i], prevDim);
            taskQueue* tQ = createTaskQueue(inTiles, wTiles);

            startMult(tP, tQ, current, weights[i], layerOutputs[i]);
            broadcastBias(bP, layerOutputs[i], bias[i], inputRowSize, layers[i]);
            startActivation(aTP, layerOutputs[i], inputRowSize, layers[i], activations[i]);

            free(inTiles->tD); free(inTiles);
            free(wTiles->tD); free(wTiles);
            free(tQ->tks); free(tQ);

            current = layerOutputs[i];
        }

        return current;
    }

    ~newModel(){
        for(size_t i = 0; i < layers.size(); i++){
            freeMatrix(weights[i], layers[i]);
            freeMatrix(gradients[i], layers[i]);
            freeMatrix(bias[i], layers[i]);
            freeMatrix(biasGrads[i], layers[i]);
            freeMatrix(layerOutputs[i], inputRowSize);
        }
        delete[] weights;
        delete[] gradients;
        delete[] bias;
        delete[] biasGrads;
        delete[] layerOutputs;

        deleteMultPool(tP);
        deleteBiasPool(bP);
        deleteActPool(aTP);
    }

};

int main(){
    std::vector<int> layerSizes = {6, 3};
    std::vector<activation> acts = {relu, sigmoid};
    int batchSize = 5;
    int inputFeatures = 4;

    newModel model(layerSizes, batchSize, inputFeatures, acts);

    double** input = createMatrix(batchSize, inputFeatures);
    double** output = model.forward(input);

    std::cout << "Forward pass output (" << batchSize << "x" << layerSizes.back() << "):\n";
    for(int i = 0; i < batchSize; i++){
        for(int j = 0; j < layerSizes.back(); j++){
            std::cout << output[i][j] << " ";
        }
        std::cout << "\n";
    }

    freeMatrix(input, batchSize);
    return 0;
}
