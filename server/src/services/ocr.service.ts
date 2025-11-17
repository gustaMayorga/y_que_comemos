import Tesseract from 'tesseract.js';

export async function extractTextFromImage(imagePath: string): Promise<string> {
  try {
    const { data } = await Tesseract.recognize(imagePath, 'spa', {
      logger: (m) => {
        if (m.status === 'recognizing text') {
          console.log(`OCR Progress: ${(m.progress * 100).toFixed(0)}%`);
        }
      },
    });

    return data.text;
  } catch (error) {
    console.error('Error extracting text from image:', error);
    throw new Error('No se pudo extraer texto de la imagen');
  }
}

export function parseReceiptText(text: string): Array<{
  name: string;
  quantity: number;
  unit: string;
  price?: number;
}> {
  // This is a simplified parser. In production, you'd use more sophisticated
  // text parsing or train a model specifically for receipt parsing
  const lines = text.split('\n');
  const items: Array<any> = [];

  for (const line of lines) {
    // Simple regex to match common receipt patterns
    // Example: "Tomates 2 kg $500"
    const match = line.match(/([A-Za-zñáéíóú\s]+)\s+(\d+(?:\.\d+)?)\s*(\w+)/i);

    if (match) {
      const [, name, quantity, unit] = match;
      items.push({
        name: name.trim(),
        quantity: parseFloat(quantity),
        unit: unit.toLowerCase(),
      });
    }
  }

  return items;
}
