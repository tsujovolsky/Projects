{
 "cells": [
  {
   "cell_type": "code",
   "execution_count": 34,
   "metadata": {},
   "outputs": [],
   "source": [
    "import pandas as pd\n",
    "from sklearn.neighbors import KNeighborsClassifier\n",
    "import numpy as np\n",
    "import itertools \n",
    "import metnum\n",
    "from sklearn.metrics import accuracy_score\n",
    "from sklearn.metrics import precision_score\n",
    "from sklearn.metrics import recall_score\n",
    "\n",
    "# precondicion: K tiene que ser divisor de df.count (o cambiar las cuentas)\n",
    "def KfoldXvalidation(df, K, knn, pca):\n",
    "    images = df_train[df_train.columns[1:]].values\n",
    "    labels = df_train[\"label\"].values.reshape(-1, 1)\n",
    "    images_count = images.shape[0]\n",
    "    images_per_fold = int(images_count/K)\n",
    "    accuracies = []\n",
    "    precisions = []\n",
    "    recalls = []\n",
    "    for i in range(0, K):\n",
    "        low_lim = i * images_per_fold\n",
    "        high_lim = low_lim + images_per_fold\n",
    "        \n",
    "        images_train_low, train_labels_low = images[:low_lim], labels[:low_lim]\n",
    "        images_train_high, train_labels_high = images[high_lim:], labels[high_lim:]\n",
    "        \n",
    "        images_train = np.concatenate((images_train_low,images_train_high))\n",
    "        train_labels = np.concatenate((train_labels_low,train_labels_high))\n",
    "        images_test, test_labels = images[low_lim:high_lim], labels[low_lim:high_lim]\n",
    "        \n",
    "        if pca > 0:\n",
    "            pca_train = metnum.PCA(pca)\n",
    "            pca_train.fit(images_train)\n",
    "            images_train = pca_train.transform(images_train)\n",
    "            images_test = pca_train.transform(images_test)\n",
    "        \n",
    "        clf = metnum.KNNClassifier(knn)\n",
    "        clf.fit(images_train, train_labels)\n",
    "        \n",
    "        prediction = clf.predict(images_test)\n",
    "        \n",
    "        accuracy = accuracy_score(test_labels, prediction)\n",
    "        \n",
    "        print(accuracy)\n",
    "            \n",
    "\n",
    "#F-Score function\n",
    "def F(beta, precision, recall):\n",
    "    return (1 + beta)*(precision * recall)*((beta**2)*precision+recall)\n",
    "        "
   ]
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "Python 3",
   "language": "python",
   "name": "python3"
  },
  "language_info": {
   "codemirror_mode": {
    "name": "ipython",
    "version": 3
   },
   "file_extension": ".py",
   "mimetype": "text/x-python",
   "name": "python",
   "nbconvert_exporter": "python",
   "pygments_lexer": "ipython3",
   "version": "3.7.9"
  }
 },
 "nbformat": 4,
 "nbformat_minor": 2
}
