# Proof of Concepts

De proof of concepts ben ik vergeten tonen in de fysieke lessen.

Bij de POC van de h brug is het enige wat niet werkt dat de linker motor niet achteruit kan draaien.
Dit komt omdat voor die motor achteruit te laten draaien er op de h brug bij Ain2 een hoog signaal moet zijn.
Deze is aangesloten op digitale pin18 van de esp en deze werkt jammer genoeg niet wanneer de SPI in gebruik is.
De SPI is in gebruik omdat ik met bluetooth werk. Ook als ik deze verwijder werkt het niet omdat de SPI automatisch opgestart wordt.
