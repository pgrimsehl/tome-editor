#include "projectcontroller.h"

#include <QTextStream>

#include "projectserializer.h"
#include "../Model/project.h"
#include "../../Components/Controller/componentsetserializer.h"
#include "../../Export/Controller/exporttemplateserializer.h"
#include "../../Fields/Controller/fielddefinitionsetserializer.h"
#include "../../Import/Controller/importtemplateserializer.h"
#include "../../Records/Controller/recordsetserializer.h"
#include "../../Types/Controller/customtypesetserializer.h"
#include "../../../Util/pathutils.h"


using namespace Tome;


const QString ProjectController::ComponentFileExtension = ".tcomp";
const QString ProjectController::FieldDefinitionFileExtension = ".tfields";
const QString ProjectController::ProjectFileExtension = ".tproj";
const QString ProjectController::RecordFileExtension = ".tdata";
const QString ProjectController::RecordExportComponentTemplateExtension = ".texportc";
const QString ProjectController::RecordExportComponentDelimiterExtension = ".texportcd";
const QString ProjectController::RecordExportFieldValueTemplateExtension = ".texportv";
const QString ProjectController::RecordExportFieldValueDelimiterExtension = ".texportvd";
const QString ProjectController::RecordExportListTemplateExtension = ".texportl";
const QString ProjectController::RecordExportListItemTemplateExtension = ".texportli";
const QString ProjectController::RecordExportListItemDelimiterExtension = ".texportld";
const QString ProjectController::RecordExportLocalizedFieldValueTemplateExtension = ".texportvloc";
const QString ProjectController::RecordExportMapTemplateExtension = ".texportm";
const QString ProjectController::RecordExportMapItemTemplateExtension = ".texportmi";
const QString ProjectController::RecordExportMapItemDelimiterExtension = ".texportmd";
const QString ProjectController::RecordExportRecordFileTemplateExtension = ".texportf";
const QString ProjectController::RecordExportRecordTemplateExtension = ".texportr";
const QString ProjectController::RecordExportRecordDelimiterExtension = ".texportrd";
const QString ProjectController::RecordExportTemplateFileExtension = ".texport";
const QString ProjectController::RecordImportTemplateFileExtension = ".timport";
const QString ProjectController::TypeFileExtension = ".ttypes";


ProjectController::ProjectController() :
    recordSetSerializer(new RecordSetSerializer())
{
    // Connect signals.
    connect(
                this->recordSetSerializer,
                SIGNAL(progressChanged(QString, QString, int, int)),
                SLOT(onProgressChanged(QString, QString, int, int))
                );
}

ProjectController::~ProjectController()
{
    delete this->recordSetSerializer;
}

QString ProjectController::buildFullFilePath(QString filePath, QString projectPath, QString desiredExtension) const
{
    if (!filePath.endsWith(desiredExtension))
    {
        filePath = filePath + desiredExtension;
    }

    if (QDir::isRelativePath(filePath))
    {
        filePath = combinePaths(projectPath, filePath);
    }

    return filePath;
}

void ProjectController::createProject(const QString& projectName, const QString& projectPath, const RecordIdType::RecordIdType recordIdType)
{
    qInfo(qUtf8Printable(QString("Creating new project %1 at %2.").arg(projectName, projectPath)));

    // Create new project.
    QSharedPointer<Project> newProject = QSharedPointer<Project>::create();
    newProject->name = projectName;
    newProject->path = projectPath;
    newProject->recordIdType = recordIdType;

    // Create field definition set.
    FieldDefinitionSet fieldDefinitionSet = FieldDefinitionSet();
    fieldDefinitionSet.name = projectName;
    newProject->fieldDefinitionSets.push_back(fieldDefinitionSet);

    // Create record set.
    RecordSet recordSet = RecordSet();
    recordSet.name = projectName;
    newProject->recordSets.push_back(recordSet);

    // Create component set.
    ComponentSet componentSet = ComponentSet();
    componentSet.name = projectName;
    newProject->componentSets.push_back(componentSet);

    // Create custom type set.
    CustomTypeSet customTypeSet = CustomTypeSet();
    customTypeSet.name = projectName;
    newProject->typeSets.push_back(customTypeSet);

    // Write project files.
    this->saveProject(newProject);

    // Set project reference.
    this->setProject(newProject);
}

const QString ProjectController::getFullProjectPath() const
{
    return this->getFullProjectPath(this->project);
}


const QString ProjectController::getProjectName() const
{
    return this->project->name;
}

const QString ProjectController::getProjectPath() const
{
    return this->project->path;
}

RecordIdType::RecordIdType ProjectController::getProjectRecordIdType() const
{
    return this->project->recordIdType;
}

bool ProjectController::getProjectIgnoreReadOnly() const
{
    return this->project->ignoreReadOnly;
}

bool ProjectController::hasComponentSet(const QString &componentSetName) const
{
    for (auto it = this->project->componentSets.cbegin();
         it != this->project->componentSets.cend();
         ++it)
    {
        if ((*it).name == componentSetName)
        {
            return true;
        }
    }

    return false;
}

bool ProjectController::hasCustomTypeSet(const QString &customTypeSetName) const
{
    for (auto it = this->project->typeSets.cbegin();
         it != this->project->typeSets.cend();
         ++it)
    {
        if ((*it).name == customTypeSetName)
        {
            return true;
        }
    }

    return false;
}

bool ProjectController::hasExportTemplate(const QString &exportTemplatePath) const
{
    for (auto it = this->project->recordExportTemplates.cbegin();
         it != this->project->recordExportTemplates.cend();
         ++it)
    {
        if ((*it).path == exportTemplatePath)
        {
            return true;
        }
    }

    return false;
}

bool ProjectController::hasFieldDefinitionSet(const QString &fieldDefinitionSetName) const
{
    for (auto it = this->project->fieldDefinitionSets.cbegin();
         it != this->project->fieldDefinitionSets.cend();
         ++it)
    {
        if ((*it).name == fieldDefinitionSetName)
        {
            return true;
        }
    }

    return false;
}

bool ProjectController::hasImportTemplate(const QString &importTemplatePath) const
{
    for (auto it = this->project->recordTableImportTemplates.cbegin();
         it != this->project->recordTableImportTemplates.cend();
         ++it)
    {
        if ((*it).path == importTemplatePath)
        {
            return true;
        }
    }

    return false;
}

bool ProjectController::hasRecordSet(const QString &recordSetName) const
{
    for (auto it = this->project->recordSets.cbegin();
         it != this->project->recordSets.cend();
         ++it)
    {
        if ((*it).name == recordSetName)
        {
            return true;
        }
    }

    return false;
}

bool ProjectController::isProjectLoaded() const
{
    return this->project != 0;
}

void ProjectController::loadComponentSet(const QString& projectPath, ComponentSet& componentSet) const
{
    ComponentSetSerializer componentSerializer = ComponentSetSerializer();

    // Open component file.
    QString fullComponentSetPath =
            buildFullFilePath(componentSet.name, projectPath, ComponentFileExtension);

    QFile componentFile(fullComponentSetPath);

    qInfo(qUtf8Printable(QString("Opening components file %1.").arg(fullComponentSetPath)));

    if (componentFile.open(QIODevice::ReadOnly))
    {
        try
        {
            componentSerializer.deserialize(componentFile, componentSet);
            qInfo(qUtf8Printable(QString("Opened components file %1 with %2 components.")
                  .arg(fullComponentSetPath, QString::number(componentSet.components.count()))));
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + fullComponentSetPath + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
    else
    {
        QString errorMessage = QObject::tr("File could not be read:\r\n") + fullComponentSetPath;
        throw std::runtime_error(errorMessage.toStdString());
    }
}

void ProjectController::loadCustomTypeSet(const QString& projectPath, CustomTypeSet& typeSet) const
{
    CustomTypeSetSerializer typesSerializer = CustomTypeSetSerializer();

    // Open types file.
    QString fullTypeSetPath =
            buildFullFilePath(typeSet.name, projectPath, TypeFileExtension);

    QFile typeFile(fullTypeSetPath);

    qInfo(qUtf8Printable(QString("Opening types file %1.").arg(fullTypeSetPath)));

    if (typeFile.open(QIODevice::ReadOnly))
    {
        try
        {
            typesSerializer.deserialize(typeFile, typeSet);
            qInfo(qUtf8Printable(QString("Opened types file %1 with %2 custom types.")
                  .arg(fullTypeSetPath, QString::number(typeSet.types.count()))));
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + fullTypeSetPath + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
    else
    {
        QString errorMessage = QObject::tr("File could not be read:\r\n") + fullTypeSetPath;
        throw std::runtime_error(errorMessage.toStdString());
    }
}

void ProjectController::loadExportTemplate(const QString& projectPath, RecordExportTemplate& exportTemplate) const
{
    ExportTemplateSerializer exportTemplateSerializer = ExportTemplateSerializer();

    // Read template file.
    QString fullExportTemplatePath =
            buildFullFilePath(exportTemplate.path, projectPath, RecordExportTemplateFileExtension);

    QFile exportTemplateFile(fullExportTemplatePath);

    qInfo(qUtf8Printable(QString("Opening export template file %1.").arg(fullExportTemplatePath)));

    if (exportTemplateFile.open(QIODevice::ReadOnly))
    {
        try
        {
            exportTemplateSerializer.deserialize(exportTemplateFile, exportTemplate);
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + fullExportTemplatePath + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
    else
    {
        QString errorMessage = QObject::tr("File could not be read:\r\n") + fullExportTemplatePath;
        throw std::runtime_error(errorMessage.toStdString());
    }

    // Read template contents.
    try
    {
        QString templatePath = exportTemplate.path;

        if (QDir::isRelativePath(templatePath))
        {
            templatePath = combinePaths(projectPath, templatePath);
        }

        if (templatePath.endsWith(RecordExportTemplateFileExtension))
        {
            templatePath = templatePath.remove(RecordExportTemplateFileExtension);
        }

        exportTemplate.fieldValueDelimiter =
                this->readFile(templatePath + RecordExportFieldValueDelimiterExtension);
        exportTemplate.fieldValueTemplate =
                this->readFile(templatePath + RecordExportFieldValueTemplateExtension);
        exportTemplate.recordDelimiter =
                this->readFile(templatePath + RecordExportRecordDelimiterExtension);
        exportTemplate.recordFileTemplate =
                this->readFile(templatePath + RecordExportRecordFileTemplateExtension);
        exportTemplate.recordTemplate =
                this->readFile(templatePath + RecordExportRecordTemplateExtension);
        exportTemplate.componentDelimiter =
                this->readFile(templatePath + RecordExportComponentDelimiterExtension);
        exportTemplate.componentTemplate =
                this->readFile(templatePath + RecordExportComponentTemplateExtension);
        exportTemplate.listTemplate =
                this->readFile(templatePath + RecordExportListTemplateExtension);
        exportTemplate.listItemTemplate =
                this->readFile(templatePath + RecordExportListItemTemplateExtension);
        exportTemplate.listItemDelimiter =
                this->readFile(templatePath + RecordExportListItemDelimiterExtension);
        exportTemplate.mapTemplate =
                this->readFile(templatePath + RecordExportMapTemplateExtension);
        exportTemplate.mapItemTemplate =
                this->readFile(templatePath + RecordExportMapItemTemplateExtension);
        exportTemplate.mapItemDelimiter =
                this->readFile(templatePath + RecordExportMapItemDelimiterExtension);
        exportTemplate.localizedFieldValueTemplate =
                this->readFile(templatePath + RecordExportLocalizedFieldValueTemplateExtension);
    }
    catch (const std::runtime_error& e)
    {
        QString errorMessage = QObject::tr("Export template %1 is missing a required file: %2")
                .arg(exportTemplate.name, e.what());
        qCritical(qUtf8Printable(errorMessage));
    }
}

void ProjectController::loadFieldDefinitionSet(const QString& projectPath, FieldDefinitionSet& fieldDefinitionSet) const
{
    FieldDefinitionSetSerializer fieldDefinitionSerializer = FieldDefinitionSetSerializer();

    // Open field definition file.
    QString fullFieldDefinitionSetPath =
            buildFullFilePath(fieldDefinitionSet.name, projectPath, FieldDefinitionFileExtension);

    QFile fieldDefinitionFile(fullFieldDefinitionSetPath);

    qInfo(qUtf8Printable(QString("Opening field definitions file %1.").arg(fullFieldDefinitionSetPath)));

    if (fieldDefinitionFile.open(QIODevice::ReadOnly))
    {
        try
        {
            fieldDefinitionSerializer.deserialize(fieldDefinitionFile, fieldDefinitionSet);
            qInfo(qUtf8Printable(QString("Opened field definitions file %1 with %2 fields.")
                  .arg(fullFieldDefinitionSetPath, QString::number(fieldDefinitionSet.fieldDefinitions.count()))));
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + fullFieldDefinitionSetPath + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
    else
    {
        QString errorMessage = QObject::tr("File could not be read:\r\n") + fullFieldDefinitionSetPath;
        throw std::runtime_error(errorMessage.toStdString());
    }
}

void ProjectController::loadImportTemplate(const QString& projectPath, RecordTableImportTemplate& importTemplate) const
{
    ImportTemplateSerializer importTemplateSerializer = ImportTemplateSerializer();

    // Open import template file.
    QString fullImportTemplatePath =
            buildFullFilePath(importTemplate.path, projectPath, RecordImportTemplateFileExtension);

    QFile importTemplateFile(fullImportTemplatePath);

    qInfo(qUtf8Printable(QString("Opening import template file %1.").arg(fullImportTemplatePath)));

    if (importTemplateFile.open(QIODevice::ReadOnly))
    {
        try
        {
            importTemplateSerializer.deserialize(importTemplateFile, importTemplate);
            qInfo(qUtf8Printable(QString("Opened import template file %1.")
                  .arg(fullImportTemplatePath)));
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + fullImportTemplatePath + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
    else
    {
        QString errorMessage = QObject::tr("File could not be read:\r\n") + fullImportTemplatePath;
        throw std::runtime_error(errorMessage.toStdString());
    }
}

void ProjectController::loadRecordSet(const QString& projectPath, RecordSet& recordSet) const
{
    // Open record file.
    QString fullRecordSetPath =
            buildFullFilePath(recordSet.name, projectPath, RecordFileExtension);

    QFile recordFile(fullRecordSetPath);

    qInfo(qUtf8Printable(QString("Opening records file %1.").arg(fullRecordSetPath)));

    if (recordFile.open(QIODevice::ReadOnly))
    {
        try
        {
            this->recordSetSerializer->deserialize(recordFile, recordSet);
            qInfo(qUtf8Printable(QString("Opened records file %1 with %2 records.")
                  .arg(fullRecordSetPath, QString::number(recordSet.records.count()))));
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + fullRecordSetPath + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
    else
    {
        QString errorMessage = QObject::tr("File could not be read:\r\n") + fullRecordSetPath;
        throw std::runtime_error(errorMessage.toStdString());
    }
}

void ProjectController::openProject(const QString& projectFileName)
{
    if (projectFileName.count() <= 0)
    {
        return;
    }

    // Open project file.
    QFile projectFile(projectFileName);
    QFileInfo projectFileInfo(projectFileName);

    const QString projectPath = projectFileInfo.path();

    qInfo(qUtf8Printable(QString("Opening project %1.").arg(projectFileName)));

    if (projectFile.open(QIODevice::ReadOnly))
    {
        // Load project from file.
        ProjectSerializer projectSerializer = ProjectSerializer();
        QSharedPointer<Project> project = QSharedPointer<Project>::create();
        project->path = projectPath;

        try
        {
            projectSerializer.deserialize(projectFile, project);
        }
        catch (const std::runtime_error& e)
        {
            QString errorMessage = QObject::tr("File could not be read: ") + projectFileName + "\r\n" + e.what();
            qCritical(qUtf8Printable(errorMessage));
            throw std::runtime_error(errorMessage.toStdString());
        }

        // Load component files.
        for (int i = 0; i < project->componentSets.size(); ++i)
        {
            this->loadComponentSet(projectPath, project->componentSets[i]);
        }

        // Load field definition files.
        for (int i = 0; i < project->fieldDefinitionSets.size(); ++i)
        {
            this->loadFieldDefinitionSet(projectPath, project->fieldDefinitionSets[i]);
        }

        // Load record files.
        for (int i = 0; i < project->recordSets.size(); ++i)
        {
            this->loadRecordSet(projectPath, project->recordSets[i]);
        }

        // Load record export template files.
        for (RecordExportTemplateList::iterator it = project->recordExportTemplates.begin();
             it != project->recordExportTemplates.end();
             ++it)
        {
            this->loadExportTemplate(projectPath, *it);
        }

        // Load type files.
        for (int i = 0; i < project->typeSets.size(); ++i)
        {
            this->loadCustomTypeSet(projectPath, project->typeSets[i]);
        }

        // Load record import template files.
        for (RecordTableImportTemplateList::iterator it = project->recordTableImportTemplates.begin();
             it != project->recordTableImportTemplates.end();
             ++it)
        {
            this->loadImportTemplate(projectPath, *it);
        }

        // Set project reference.
        this->setProject(project);
    }
    else
    {
        // Throw exception.
        QString errorMessage = QObject::tr("File could not be read:\r\n") + projectFileName;
        throw std::runtime_error(errorMessage.toStdString());
    }
}

void ProjectController::saveProject() const
{
    this->saveProject(this->project);
}

void ProjectController::onProgressChanged(const QString title, const QString text, const int currentValue, const int maximumValue)
{
    emit this->progressChanged(title, text, currentValue, maximumValue);
}

const QString ProjectController::getFullProjectPath(QSharedPointer<Project> project) const
{
    if (project == 0)
    {
        return QString();
    }

    return combinePaths(project->path, project->name + ProjectFileExtension);
}

QString ProjectController::readFile(const QString& fullPath) const
{
    QFile file(fullPath);

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        return textStream.readAll();
    }
    else
    {
        throw std::runtime_error(fullPath.toStdString());
    }
}

void ProjectController::saveProject(QSharedPointer<Project> project) const
{
    QString& projectPath = project->path;
    ProjectSerializer projectSerializer = ProjectSerializer();

    // Build file name.
    const QString fullProjectPath = this->getFullProjectPath(project);

    // Write project file.
    QFile projectFile(fullProjectPath);

    qInfo(qUtf8Printable(QString("Saving project %1.").arg(fullProjectPath)));

    if (projectFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        projectSerializer.serialize(projectFile, project);
    }
    else
    {
        QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullProjectPath;
        throw std::runtime_error(errorMessage.toStdString());
    }

    // Write component sets.
    ComponentSetSerializer componentSetSerializer = ComponentSetSerializer();

    for (int i = 0; i < project->componentSets.size(); ++i)
    {
        const ComponentSet& componentSet = project->componentSets[i];

        // Build file name.
        QString fullComponentSetPath =
                buildFullFilePath(componentSet.name, projectPath, ComponentFileExtension);

        // Write file.
        QFile componentSetFile(fullComponentSetPath);

        qInfo(qUtf8Printable(QString("Saving components file %1.").arg(fullComponentSetPath)));

        if (componentSetFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            componentSetSerializer.serialize(componentSetFile, componentSet);
        }
        else
        {
            QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullComponentSetPath;
            throw std::runtime_error(errorMessage.toStdString());
        }
    }

    // Write field definition sets.
    FieldDefinitionSetSerializer fieldDefinitionSetSerializer = FieldDefinitionSetSerializer();

    for (int i = 0; i < project->fieldDefinitionSets.size(); ++i)
    {
        const FieldDefinitionSet& fieldDefinitionSet = project->fieldDefinitionSets[i];

        // Build file name.
        QString fullFieldDefinitionSetPath =
                buildFullFilePath(fieldDefinitionSet.name, projectPath, FieldDefinitionFileExtension);

        // Write file.
        QFile fieldDefinitionSetFile(fullFieldDefinitionSetPath);

        qInfo(qUtf8Printable(QString("Saving field definitions file %1.").arg(fullFieldDefinitionSetPath)));

        if (fieldDefinitionSetFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            fieldDefinitionSetSerializer.serialize(fieldDefinitionSetFile, fieldDefinitionSet);
        }
        else
        {
            QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullFieldDefinitionSetPath;
            throw std::runtime_error(errorMessage.toStdString());
        }
    }

    // Write record sets.
    for (int i = 0; i < project->recordSets.size(); ++i)
    {
        const RecordSet& recordSet = project->recordSets[i];

        // Build file name.
        QString fullRecordSetPath =
                buildFullFilePath(recordSet.name, projectPath, RecordFileExtension);

        // Write file.
        QFile recordSetFile(fullRecordSetPath);

        qInfo(qUtf8Printable(QString("Saving records file %1.").arg(fullRecordSetPath)));

        if (recordSetFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            this->recordSetSerializer->serialize(recordSetFile, recordSet);
        }
        else
        {
            QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullRecordSetPath;
            throw std::runtime_error(errorMessage.toStdString());
        }
    }

    // Write export templates.
    ExportTemplateSerializer exportTemplateSerializer = ExportTemplateSerializer();

    for (RecordExportTemplateList::const_iterator it = project->recordExportTemplates.cbegin();
         it != project->recordExportTemplates.cend();
         ++it)
    {
        const RecordExportTemplate& exportTemplate = *it;

        // Build file name.
        QString fullExportTemplatePath =
                buildFullFilePath(exportTemplate.path, projectPath, RecordExportTemplateFileExtension);

        // Write file.
        QFile exportTemplateFile(fullExportTemplatePath);

        qInfo(qUtf8Printable(QString("Saving export template file %1.").arg(fullExportTemplatePath)));

        if (exportTemplateFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            exportTemplateSerializer.serialize(exportTemplateFile, exportTemplate);
        }
        else
        {
            QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullExportTemplatePath;
            throw std::runtime_error(errorMessage.toStdString());
        }
    }

    // Write type sets.
    CustomTypeSetSerializer typeSetSerializer = CustomTypeSetSerializer();

    for (int i = 0; i < project->typeSets.size(); ++i)
    {
        const CustomTypeSet& typeSet = project->typeSets[i];

        // Build file name.
        QString fullTypeSetPath =
                buildFullFilePath(typeSet.name, projectPath, TypeFileExtension);

        // Write file.
        QFile typeSetFile(fullTypeSetPath);

        qInfo(qUtf8Printable(QString("Saving types file %1.").arg(fullTypeSetPath)));

        if (typeSetFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            typeSetSerializer.serialize(typeSetFile, typeSet);
        }
        else
        {
            QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullTypeSetPath;
            throw std::runtime_error(errorMessage.toStdString());
        }
    }

    // Write import templates.
    ImportTemplateSerializer importTemplateSerializer = ImportTemplateSerializer();

    for (RecordTableImportTemplateList::const_iterator it = project->recordTableImportTemplates.cbegin();
         it != project->recordTableImportTemplates.cend();
         ++it)
    {
        const RecordTableImportTemplate& importTemplate = *it;

        // Build file name.
        QString fullImportTemplatePath =
                buildFullFilePath(importTemplate.path, projectPath, RecordImportTemplateFileExtension);

        // Write file.
        QFile importTemplateFile(fullImportTemplatePath);

        qInfo(qUtf8Printable(QString("Saving import template file %1.").arg(fullImportTemplatePath)));

        if (importTemplateFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            importTemplateSerializer.serialize(importTemplateFile, importTemplate);
        }
        else
        {
            QString errorMessage = QObject::tr("Destination file could not be written:\r\n") + fullImportTemplatePath;
            throw std::runtime_error(errorMessage.toStdString());
        }
    }
}

void ProjectController::setProject(QSharedPointer<Project> project)
{
    this->project = project;

    // Notify listeners.
    emit projectChanged(this->project);
}

