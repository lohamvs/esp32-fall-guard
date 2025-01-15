from pymongo import MongoClient

conexao = MongoClient("mongodb://admin:password@localhost:27017/")
banco = conexao["sensor"]
colecao = banco["mpu6050"]

atividadesNormais = (
    10 +  # 10 vezes sentando e levantando
    10 +  # 10 vezes pegando objetos no chão
    10    # 10 vezes deitando rapidamente
)

quedasDetectadas = colecao.count_documents({"isAlert": True, "isActualFall": True})
quedasPerdidas = colecao.count_documents({"isAlert": False, "isActualFall": True})
falsosAlertas = colecao.count_documents({"isAlert": True, "isActualFall": {"$ne": True}})
naoQuedasCorretas = atividadesNormais - falsosAlertas

totalCasos = quedasDetectadas + quedasPerdidas + falsosAlertas + naoQuedasCorretas

sensibilidade = quedasDetectadas / (quedasDetectadas + quedasPerdidas) if (quedasDetectadas + quedasPerdidas) > 0 else 0
especificidade = naoQuedasCorretas / (naoQuedasCorretas + falsosAlertas) if (naoQuedasCorretas + falsosAlertas) > 0 else 0
acuracia = (quedasDetectadas + naoQuedasCorretas) / totalCasos if totalCasos > 0 else 0

print("Resultados da análise:")
print(f"- Quedas Detectadas (TP): {quedasDetectadas}")
print(f"- Quedas Perdidas (FN): {quedasPerdidas}")
print(f"- Falsos Alertas (FP): {falsosAlertas}")
print(f"- Não Quedas Corretas (TN): {naoQuedasCorretas}")
print(f"- Sensibilidade: {sensibilidade:.2f}")
print(f"- Especificidade: {especificidade:.2f}")
print(f"- Acurácia: {acuracia:.2f}")
