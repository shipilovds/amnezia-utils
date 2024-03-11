#include <iostream>
#include <fstream>
#include <sstream>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QDebug>
#include <QDataStream>
#include <zlib.h>

QString encodedAmneziaConfig(const QByteArray &jsonData) {
    // Compress JSON data
    QByteArray compressedConfig = qCompress(jsonData, 8);

    // Convert to Base64UrlEncoding and OmitTrailingEquals
    QString result = QString("vpn://%1")
                         .arg(QString(compressedConfig.toBase64(QByteArray::Base64UrlEncoding |
                                                                  QByteArray::OmitTrailingEquals)));
    return result;
}

QJsonObject extractAmneziaConfig(const QString &connectionString) {
    QString rawAmneziaConfig = connectionString;
    rawAmneziaConfig.replace("vpn://", "");
    QByteArray base64Decoded = QByteArray::fromBase64(rawAmneziaConfig.toUtf8(), QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    QByteArray fullyDecoded = qUncompress(base64Decoded);
    QJsonObject config = QJsonDocument::fromJson(fullyDecoded).object();

    return config;
}

QString readInputFile(const QString &filename) {
    if (filename.isEmpty() || filename == "-") {
        std::stringstream buffer;
        buffer << std::cin.rdbuf();
        return QString::fromStdString(buffer.str());
    } else {
        std::ifstream inputFile(filename.toStdString(), std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error: Unable to open file " << filename.toStdString() << std::endl;
            return QString();
        }

        std::stringstream buffer;
        buffer << inputFile.rdbuf();
        return QString::fromStdString(buffer.str());
    }
}

QByteArray readJsonData() {
    // Read JSON data from standard input
    std::stringstream buffer;
    buffer << std::cin.rdbuf();
    return QByteArray(buffer.str().c_str());
}

QByteArray readJsonFile(const QString &filePath) {
    if (filePath.isEmpty() || filePath == "-") {
        // Read JSON data from standard input
        return readJsonData();
    } else {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error opening JSON file " << filePath.toStdString() << std::endl;
            return QByteArray();
        }

        QByteArray jsonData = file.readAll();
        file.close();

        return jsonData;
    }
}

void encoder(const QString &filePath) {
    QByteArray jsonData = readJsonFile(filePath);

    if (jsonData.isEmpty()) {
        std::cerr << "Error reading JSON data" << std::endl;
        std::exit(1);
    }
    QString encoded = encodedAmneziaConfig(jsonData);

    // Print result to stdout
    std::cout << encoded.toStdString() << std::endl;
}

void decoder(const QString &filePath) {
    const QString inputData = readInputFile(filePath);

    if (inputData.isEmpty()) {
        std::cerr << "Error reading input data" << std::endl;
        std::exit(1);
    }

    QJsonObject result = extractAmneziaConfig(inputData);
    QJsonDocument doc(result);
    std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
}

void printHelp(const char *programName) {
    std::cerr << "Usage: " << programName << " [-e|-d] [input_file|-]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -e              Encode JSON data" << std::endl;
    std::cerr << "  -d              Decode Amnezia configuration" << std::endl;
    std::cerr << "  -h              Display this help message" << std::endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc < 2) {
        printHelp(argv[0]);
        return 1;
    }

    QString option = argv[1];
    QString filePath = (argc == 3) ? argv[2] : "";

    if (option == "-e") {
        encoder(filePath);
    } else if (option == "-d") {
        decoder(filePath);
    } else if (option == "-h") {
        printHelp(argv[0]);
    } else {
        std::cerr << "Error: Invalid option" << std::endl;
        printHelp(argv[0]);
        return 1;
    }

    return 0;
}

