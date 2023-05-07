const sheet_id = '17XQNBk0kwROMUUCM5NZ6weJQE-_mAdU8Dz5HYLnDZ28';
// Se obtiene la hoja de cálculo activa
let sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();		
// Se obtiene el número de la última fila ocupada en la hoja y se agrega una fila

function doGet(e){
  var read = e.parameter.read;

  if (read !== undefined){
    // Obtener los últimos 3 registros de la hoja
    var range = sheet.getRange(sheet.getLastRow() - 2, 1, 3, sheet.getLastColumn());
    var values = range.getValues();

    // Crear un objeto JSON con los datos a enviar
    var jsonData = {values: values};

    // Convertir el objeto JSON en una cadena de texto
    var data = JSON.stringify(jsonData);

    // Crear la respuesta HTTP con los datos a enviar
    return ContentService.createTextOutput(data);
  }
}